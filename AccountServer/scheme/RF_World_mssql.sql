USE [master]
GO
/****** Object:  Database [RF_World]    Script Date: 12/01/2026 16:57:18 ******/
CREATE DATABASE [RF_World]
GO
ALTER DATABASE [RF_World] SET COMPATIBILITY_LEVEL = 100
GO
IF (1 = FULLTEXTSERVICEPROPERTY('IsFullTextInstalled'))
begin
EXEC [RF_World].[dbo].[sp_fulltext_database] @action = 'disable'
end
GO
ALTER DATABASE [RF_World] SET ANSI_NULL_DEFAULT OFF 
GO
ALTER DATABASE [RF_World] SET ANSI_NULLS OFF 
GO
ALTER DATABASE [RF_World] SET ANSI_PADDING OFF 
GO
ALTER DATABASE [RF_World] SET ANSI_WARNINGS OFF 
GO
ALTER DATABASE [RF_World] SET ARITHABORT OFF 
GO
ALTER DATABASE [RF_World] SET AUTO_CLOSE OFF 
GO
ALTER DATABASE [RF_World] SET AUTO_SHRINK OFF 
GO
ALTER DATABASE [RF_World] SET AUTO_UPDATE_STATISTICS ON 
GO
ALTER DATABASE [RF_World] SET CURSOR_CLOSE_ON_COMMIT OFF 
GO
ALTER DATABASE [RF_World] SET CURSOR_DEFAULT  GLOBAL 
GO
ALTER DATABASE [RF_World] SET CONCAT_NULL_YIELDS_NULL OFF 
GO
ALTER DATABASE [RF_World] SET NUMERIC_ROUNDABORT OFF 
GO
ALTER DATABASE [RF_World] SET QUOTED_IDENTIFIER OFF 
GO
ALTER DATABASE [RF_World] SET RECURSIVE_TRIGGERS OFF 
GO
ALTER DATABASE [RF_World] SET  DISABLE_BROKER 
GO
ALTER DATABASE [RF_World] SET AUTO_UPDATE_STATISTICS_ASYNC OFF 
GO
ALTER DATABASE [RF_World] SET DATE_CORRELATION_OPTIMIZATION OFF 
GO
ALTER DATABASE [RF_World] SET TRUSTWORTHY OFF 
GO
ALTER DATABASE [RF_World] SET ALLOW_SNAPSHOT_ISOLATION OFF 
GO
ALTER DATABASE [RF_World] SET PARAMETERIZATION SIMPLE 
GO
ALTER DATABASE [RF_World] SET READ_COMMITTED_SNAPSHOT OFF 
GO
ALTER DATABASE [RF_World] SET HONOR_BROKER_PRIORITY OFF 
GO
ALTER DATABASE [RF_World] SET RECOVERY FULL 
GO
ALTER DATABASE [RF_World] SET  MULTI_USER 
GO
ALTER DATABASE [RF_World] SET PAGE_VERIFY CHECKSUM  
GO
ALTER DATABASE [RF_World] SET DB_CHAINING OFF 
GO
ALTER DATABASE [RF_World] SET FILESTREAM( NON_TRANSACTED_ACCESS = OFF ) 
GO
ALTER DATABASE [RF_World] SET TARGET_RECOVERY_TIME = 0 SECONDS 
GO
ALTER DATABASE [RF_World] SET DELAYED_DURABILITY = DISABLED 
GO
ALTER DATABASE [RF_World] SET ACCELERATED_DATABASE_RECOVERY = OFF  
GO
EXEC sys.sp_db_vardecimal_storage_format N'RF_World', N'ON'
GO
ALTER DATABASE [RF_World] SET QUERY_STORE = OFF
GO
USE [RF_World]
GO
/****** Object:  Table [dbo].[deleteitem]    Script Date: 12/01/2026 16:57:19 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[deleteitem](
	[itemcode] [varchar](8) NOT NULL,
	[itemkey] [int] NOT NULL,
	[itemname] [varchar](65) NULL,
	[ucode] [int] NOT NULL
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[tbl_AccountTrunk]    Script Date: 12/01/2026 16:57:19 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[tbl_AccountTrunk](
	[AccountSerial] [int] NOT NULL,
	[DCK] [bit] NOT NULL,
	[EstSlot] [tinyint] NOT NULL,
	[TrunkPass] [binary](24) NOT NULL,
	[HintIndex] [tinyint] NOT NULL,
	[HintAnswer] [varchar](17) NOT NULL,
	[K0] [int] NOT NULL,
	[D0] [bigint] NOT NULL,
	[U0] [int] NOT NULL,
	[R0] [tinyint] NOT NULL,
	[K1] [int] NOT NULL,
	[D1] [bigint] NOT NULL,
	[U1] [int] NOT NULL,
	[R1] [tinyint] NOT NULL,
	[K2] [int] NOT NULL,
	[D2] [bigint] NOT NULL,
	[U2] [int] NOT NULL,
	[R2] [tinyint] NOT NULL,
	[K3] [int] NOT NULL,
	[D3] [bigint] NOT NULL,
	[U3] [int] NOT NULL,
	[R3] [tinyint] NOT NULL,
	[K4] [int] NOT NULL,
	[D4] [bigint] NOT NULL,
	[U4] [int] NOT NULL,
	[R4] [tinyint] NOT NULL,
	[K5] [int] NOT NULL,
	[D5] [bigint] NOT NULL,
	[U5] [int] NOT NULL,
	[R5] [tinyint] NOT NULL,
	[K6] [int] NOT NULL,
	[D6] [bigint] NOT NULL,
	[U6] [int] NOT NULL,
	[R6] [tinyint] NOT NULL,
	[K7] [int] NOT NULL,
	[D7] [bigint] NOT NULL,
	[U7] [int] NOT NULL,
	[R7] [tinyint] NOT NULL,
	[K8] [int] NOT NULL,
	[D8] [bigint] NOT NULL,
	[U8] [int] NOT NULL,
	[R8] [tinyint] NOT NULL,
	[K9] [int] NOT NULL,
	[D9] [bigint] NOT NULL,
	[U9] [int] NOT NULL,
	[R9] [tinyint] NOT NULL,
	[K10] [int] NOT NULL,
	[D10] [bigint] NOT NULL,
	[U10] [int] NOT NULL,
	[R10] [tinyint] NOT NULL,
	[K11] [int] NOT NULL,
	[D11] [bigint] NOT NULL,
	[U11] [int] NOT NULL,
	[R11] [tinyint] NOT NULL,
	[K12] [int] NOT NULL,
	[D12] [bigint] NOT NULL,
	[U12] [int] NOT NULL,
	[R12] [tinyint] NOT NULL,
	[K13] [int] NOT NULL,
	[D13] [bigint] NOT NULL,
	[U13] [int] NOT NULL,
	[R13] [tinyint] NOT NULL,
	[K14] [int] NOT NULL,
	[D14] [bigint] NOT NULL,
	[U14] [int] NOT NULL,
	[R14] [tinyint] NOT NULL,
	[K15] [int] NOT NULL,
	[D15] [bigint] NOT NULL,
	[U15] [int] NOT NULL,
	[R15] [tinyint] NOT NULL,
	[K16] [int] NOT NULL,
	[D16] [bigint] NOT NULL,
	[U16] [int] NOT NULL,
	[R16] [tinyint] NOT NULL,
	[K17] [int] NOT NULL,
	[D17] [bigint] NOT NULL,
	[U17] [int] NOT NULL,
	[R17] [tinyint] NOT NULL,
	[K18] [int] NOT NULL,
	[D18] [bigint] NOT NULL,
	[U18] [int] NOT NULL,
	[R18] [tinyint] NOT NULL,
	[K19] [int] NOT NULL,
	[D19] [bigint] NOT NULL,
	[U19] [int] NOT NULL,
	[R19] [tinyint] NOT NULL,
	[K20] [int] NOT NULL,
	[D20] [bigint] NOT NULL,
	[U20] [int] NOT NULL,
	[R20] [tinyint] NOT NULL,
	[K21] [int] NOT NULL,
	[D21] [bigint] NOT NULL,
	[U21] [int] NOT NULL,
	[R21] [tinyint] NOT NULL,
	[K22] [int] NOT NULL,
	[D22] [bigint] NOT NULL,
	[U22] [int] NOT NULL,
	[R22] [tinyint] NOT NULL,
	[K23] [int] NOT NULL,
	[D23] [bigint] NOT NULL,
	[U23] [int] NOT NULL,
	[R23] [tinyint] NOT NULL,
	[K24] [int] NOT NULL,
	[D24] [bigint] NOT NULL,
	[U24] [int] NOT NULL,
	[R24] [tinyint] NOT NULL,
	[K25] [int] NOT NULL,
	[D25] [bigint] NOT NULL,
	[U25] [int] NOT NULL,
	[R25] [tinyint] NOT NULL,
	[K26] [int] NOT NULL,
	[D26] [bigint] NOT NULL,
	[U26] [int] NOT NULL,
	[R26] [tinyint] NOT NULL,
	[K27] [int] NOT NULL,
	[D27] [bigint] NOT NULL,
	[U27] [int] NOT NULL,
	[R27] [tinyint] NOT NULL,
	[K28] [int] NOT NULL,
	[D28] [bigint] NOT NULL,
	[U28] [int] NOT NULL,
	[R28] [tinyint] NOT NULL,
	[K29] [int] NOT NULL,
	[D29] [bigint] NOT NULL,
	[U29] [int] NOT NULL,
	[R29] [tinyint] NOT NULL,
	[K30] [int] NOT NULL,
	[D30] [bigint] NOT NULL,
	[U30] [int] NOT NULL,
	[R30] [tinyint] NOT NULL,
	[K31] [int] NOT NULL,
	[D31] [bigint] NOT NULL,
	[U31] [int] NOT NULL,
	[R31] [tinyint] NOT NULL,
	[K32] [int] NOT NULL,
	[D32] [bigint] NOT NULL,
	[U32] [int] NOT NULL,
	[R32] [tinyint] NOT NULL,
	[K33] [int] NOT NULL,
	[D33] [bigint] NOT NULL,
	[U33] [int] NOT NULL,
	[R33] [tinyint] NOT NULL,
	[K34] [int] NOT NULL,
	[D34] [bigint] NOT NULL,
	[U34] [int] NOT NULL,
	[R34] [tinyint] NOT NULL,
	[K35] [int] NOT NULL,
	[D35] [bigint] NOT NULL,
	[U35] [int] NOT NULL,
	[R35] [tinyint] NOT NULL,
	[K36] [int] NOT NULL,
	[D36] [bigint] NOT NULL,
	[U36] [int] NOT NULL,
	[R36] [tinyint] NOT NULL,
	[K37] [int] NOT NULL,
	[D37] [bigint] NOT NULL,
	[U37] [int] NOT NULL,
	[R37] [tinyint] NOT NULL,
	[K38] [int] NOT NULL,
	[D38] [bigint] NOT NULL,
	[U38] [int] NOT NULL,
	[R38] [tinyint] NOT NULL,
	[K39] [int] NOT NULL,
	[D39] [bigint] NOT NULL,
	[U39] [int] NOT NULL,
	[R39] [tinyint] NOT NULL,
	[K40] [int] NOT NULL,
	[D40] [bigint] NOT NULL,
	[U40] [int] NOT NULL,
	[R40] [tinyint] NOT NULL,
	[K41] [int] NOT NULL,
	[D41] [bigint] NOT NULL,
	[U41] [int] NOT NULL,
	[R41] [tinyint] NOT NULL,
	[K42] [int] NOT NULL,
	[D42] [bigint] NOT NULL,
	[U42] [int] NOT NULL,
	[R42] [tinyint] NOT NULL,
	[K43] [int] NOT NULL,
	[D43] [bigint] NOT NULL,
	[U43] [int] NOT NULL,
	[R43] [tinyint] NOT NULL,
	[K44] [int] NOT NULL,
	[D44] [bigint] NOT NULL,
	[U44] [int] NOT NULL,
	[R44] [tinyint] NOT NULL,
	[K45] [int] NOT NULL,
	[D45] [bigint] NOT NULL,
	[U45] [int] NOT NULL,
	[R45] [tinyint] NOT NULL,
	[K46] [int] NOT NULL,
	[D46] [bigint] NOT NULL,
	[U46] [int] NOT NULL,
	[R46] [tinyint] NOT NULL,
	[K47] [int] NOT NULL,
	[D47] [bigint] NOT NULL,
	[U47] [int] NOT NULL,
	[R47] [tinyint] NOT NULL,
	[K48] [int] NOT NULL,
	[D48] [bigint] NOT NULL,
	[U48] [int] NOT NULL,
	[R48] [tinyint] NOT NULL,
	[K49] [int] NOT NULL,
	[D49] [bigint] NOT NULL,
	[U49] [int] NOT NULL,
	[R49] [tinyint] NOT NULL,
	[K50] [int] NOT NULL,
	[D50] [bigint] NOT NULL,
	[U50] [int] NOT NULL,
	[R50] [tinyint] NOT NULL,
	[K51] [int] NOT NULL,
	[D51] [bigint] NOT NULL,
	[U51] [int] NOT NULL,
	[R51] [tinyint] NOT NULL,
	[K52] [int] NOT NULL,
	[D52] [bigint] NOT NULL,
	[U52] [int] NOT NULL,
	[R52] [tinyint] NOT NULL,
	[K53] [int] NOT NULL,
	[D53] [bigint] NOT NULL,
	[U53] [int] NOT NULL,
	[R53] [tinyint] NOT NULL,
	[K54] [int] NOT NULL,
	[D54] [bigint] NOT NULL,
	[U54] [int] NOT NULL,
	[R54] [tinyint] NOT NULL,
	[K55] [int] NOT NULL,
	[D55] [bigint] NOT NULL,
	[U55] [int] NOT NULL,
	[R55] [tinyint] NOT NULL,
	[K56] [int] NOT NULL,
	[D56] [bigint] NOT NULL,
	[U56] [int] NOT NULL,
	[R56] [tinyint] NOT NULL,
	[K57] [int] NOT NULL,
	[D57] [bigint] NOT NULL,
	[U57] [int] NOT NULL,
	[R57] [tinyint] NOT NULL,
	[K58] [int] NOT NULL,
	[D58] [bigint] NOT NULL,
	[U58] [int] NOT NULL,
	[R58] [tinyint] NOT NULL,
	[K59] [int] NOT NULL,
	[D59] [bigint] NOT NULL,
	[U59] [int] NOT NULL,
	[R59] [tinyint] NOT NULL,
	[K60] [int] NOT NULL,
	[D60] [bigint] NOT NULL,
	[U60] [int] NOT NULL,
	[R60] [tinyint] NOT NULL,
	[K61] [int] NOT NULL,
	[D61] [bigint] NOT NULL,
	[U61] [int] NOT NULL,
	[R61] [tinyint] NOT NULL,
	[K62] [int] NOT NULL,
	[D62] [bigint] NOT NULL,
	[U62] [int] NOT NULL,
	[R62] [tinyint] NOT NULL,
	[K63] [int] NOT NULL,
	[D63] [bigint] NOT NULL,
	[U63] [int] NOT NULL,
	[R63] [tinyint] NOT NULL,
	[K64] [int] NOT NULL,
	[D64] [bigint] NOT NULL,
	[U64] [int] NOT NULL,
	[R64] [tinyint] NOT NULL,
	[K65] [int] NOT NULL,
	[D65] [bigint] NOT NULL,
	[U65] [int] NOT NULL,
	[R65] [tinyint] NOT NULL,
	[K66] [int] NOT NULL,
	[D66] [bigint] NOT NULL,
	[U66] [int] NOT NULL,
	[R66] [tinyint] NOT NULL,
	[K67] [int] NOT NULL,
	[D67] [bigint] NOT NULL,
	[U67] [int] NOT NULL,
	[R67] [tinyint] NOT NULL,
	[K68] [int] NOT NULL,
	[D68] [bigint] NOT NULL,
	[U68] [int] NOT NULL,
	[R68] [tinyint] NOT NULL,
	[K69] [int] NOT NULL,
	[D69] [bigint] NOT NULL,
	[U69] [int] NOT NULL,
	[R69] [tinyint] NOT NULL,
	[K70] [int] NOT NULL,
	[D70] [bigint] NOT NULL,
	[U70] [int] NOT NULL,
	[R70] [tinyint] NOT NULL,
	[K71] [int] NOT NULL,
	[D71] [bigint] NOT NULL,
	[U71] [int] NOT NULL,
	[R71] [tinyint] NOT NULL,
	[K72] [int] NOT NULL,
	[D72] [bigint] NOT NULL,
	[U72] [int] NOT NULL,
	[R72] [tinyint] NOT NULL,
	[K73] [int] NOT NULL,
	[D73] [bigint] NOT NULL,
	[U73] [int] NOT NULL,
	[R73] [tinyint] NOT NULL,
	[K74] [int] NOT NULL,
	[D74] [bigint] NOT NULL,
	[U74] [int] NOT NULL,
	[R74] [tinyint] NOT NULL,
	[K75] [int] NOT NULL,
	[D75] [bigint] NOT NULL,
	[U75] [int] NOT NULL,
	[R75] [tinyint] NOT NULL,
	[K76] [int] NOT NULL,
	[D76] [bigint] NOT NULL,
	[U76] [int] NOT NULL,
	[R76] [tinyint] NOT NULL,
	[K77] [int] NOT NULL,
	[D77] [bigint] NOT NULL,
	[U77] [int] NOT NULL,
	[R77] [tinyint] NOT NULL,
	[K78] [int] NOT NULL,
	[D78] [bigint] NOT NULL,
	[U78] [int] NOT NULL,
	[R78] [tinyint] NOT NULL,
	[K79] [int] NOT NULL,
	[D79] [bigint] NOT NULL,
	[U79] [int] NOT NULL,
	[R79] [tinyint] NOT NULL,
	[K80] [int] NOT NULL,
	[D80] [bigint] NOT NULL,
	[U80] [int] NOT NULL,
	[R80] [tinyint] NOT NULL,
	[K81] [int] NOT NULL,
	[D81] [bigint] NOT NULL,
	[U81] [int] NOT NULL,
	[R81] [tinyint] NOT NULL,
	[K82] [int] NOT NULL,
	[D82] [bigint] NOT NULL,
	[U82] [int] NOT NULL,
	[R82] [tinyint] NOT NULL,
	[K83] [int] NOT NULL,
	[D83] [bigint] NOT NULL,
	[U83] [int] NOT NULL,
	[R83] [tinyint] NOT NULL,
	[K84] [int] NOT NULL,
	[D84] [bigint] NOT NULL,
	[U84] [int] NOT NULL,
	[R84] [tinyint] NOT NULL,
	[K85] [int] NOT NULL,
	[D85] [bigint] NOT NULL,
	[U85] [int] NOT NULL,
	[R85] [tinyint] NOT NULL,
	[K86] [int] NOT NULL,
	[D86] [bigint] NOT NULL,
	[U86] [int] NOT NULL,
	[R86] [tinyint] NOT NULL,
	[K87] [int] NOT NULL,
	[D87] [bigint] NOT NULL,
	[U87] [int] NOT NULL,
	[R87] [tinyint] NOT NULL,
	[K88] [int] NOT NULL,
	[D88] [bigint] NOT NULL,
	[U88] [int] NOT NULL,
	[R88] [tinyint] NOT NULL,
	[K89] [int] NOT NULL,
	[D89] [bigint] NOT NULL,
	[U89] [int] NOT NULL,
	[R89] [tinyint] NOT NULL,
	[K90] [int] NOT NULL,
	[D90] [bigint] NOT NULL,
	[U90] [int] NOT NULL,
	[R90] [tinyint] NOT NULL,
	[K91] [int] NOT NULL,
	[D91] [bigint] NOT NULL,
	[U91] [int] NOT NULL,
	[R91] [tinyint] NOT NULL,
	[K92] [int] NOT NULL,
	[D92] [bigint] NOT NULL,
	[U92] [int] NOT NULL,
	[R92] [tinyint] NOT NULL,
	[K93] [int] NOT NULL,
	[D93] [bigint] NOT NULL,
	[U93] [int] NOT NULL,
	[R93] [tinyint] NOT NULL,
	[K94] [int] NOT NULL,
	[D94] [bigint] NOT NULL,
	[U94] [int] NOT NULL,
	[R94] [tinyint] NOT NULL,
	[K95] [int] NOT NULL,
	[D95] [bigint] NOT NULL,
	[U95] [int] NOT NULL,
	[R95] [tinyint] NOT NULL,
	[K96] [int] NOT NULL,
	[D96] [bigint] NOT NULL,
	[U96] [int] NOT NULL,
	[R96] [tinyint] NOT NULL,
	[K97] [int] NOT NULL,
	[D97] [bigint] NOT NULL,
	[U97] [int] NOT NULL,
	[R97] [tinyint] NOT NULL,
	[K98] [int] NOT NULL,
	[D98] [bigint] NOT NULL,
	[U98] [int] NOT NULL,
	[R98] [tinyint] NOT NULL,
	[K99] [int] NOT NULL,
	[D99] [bigint] NOT NULL,
	[U99] [int] NOT NULL,
	[R99] [tinyint] NOT NULL,
	[Dalant0] [float] NOT NULL,
	[Gold0] [float] NOT NULL,
	[Dalant1] [float] NOT NULL,
	[Gold1] [float] NOT NULL,
	[Dalant2] [float] NOT NULL,
	[Gold2] [float] NOT NULL,
	[S0] [bigint] NOT NULL,
	[T0] [int] NOT NULL,
	[S1] [bigint] NOT NULL,
	[T1] [int] NOT NULL,
	[S2] [bigint] NOT NULL,
	[T2] [int] NOT NULL,
	[S3] [bigint] NOT NULL,
	[T3] [int] NOT NULL,
	[S4] [bigint] NOT NULL,
	[T4] [int] NOT NULL,
	[S5] [bigint] NOT NULL,
	[T5] [int] NOT NULL,
	[S6] [bigint] NOT NULL,
	[T6] [int] NOT NULL,
	[S7] [bigint] NOT NULL,
	[T7] [int] NOT NULL,
	[S8] [bigint] NOT NULL,
	[T8] [int] NOT NULL,
	[S9] [bigint] NOT NULL,
	[T9] [int] NOT NULL,
	[S10] [bigint] NOT NULL,
	[T10] [int] NOT NULL,
	[S11] [bigint] NOT NULL,
	[T11] [int] NOT NULL,
	[S12] [bigint] NOT NULL,
	[T12] [int] NOT NULL,
	[S13] [bigint] NOT NULL,
	[T13] [int] NOT NULL,
	[S14] [bigint] NOT NULL,
	[T14] [int] NOT NULL,
	[S15] [bigint] NOT NULL,
	[T15] [int] NOT NULL,
	[S16] [bigint] NOT NULL,
	[T16] [int] NOT NULL,
	[S17] [bigint] NOT NULL,
	[T17] [int] NOT NULL,
	[S18] [bigint] NOT NULL,
	[T18] [int] NOT NULL,
	[S19] [bigint] NOT NULL,
	[T19] [int] NOT NULL,
	[S20] [bigint] NOT NULL,
	[T20] [int] NOT NULL,
	[S21] [bigint] NOT NULL,
	[T21] [int] NOT NULL,
	[S22] [bigint] NOT NULL,
	[T22] [int] NOT NULL,
	[S23] [bigint] NOT NULL,
	[T23] [int] NOT NULL,
	[S24] [bigint] NOT NULL,
	[T24] [int] NOT NULL,
	[S25] [bigint] NOT NULL,
	[T25] [int] NOT NULL,
	[S26] [bigint] NOT NULL,
	[T26] [int] NOT NULL,
	[S27] [bigint] NOT NULL,
	[T27] [int] NOT NULL,
	[S28] [bigint] NOT NULL,
	[T28] [int] NOT NULL,
	[S29] [bigint] NOT NULL,
	[T29] [int] NOT NULL,
	[S30] [bigint] NOT NULL,
	[T30] [int] NOT NULL,
	[S31] [bigint] NOT NULL,
	[T31] [int] NOT NULL,
	[S32] [bigint] NOT NULL,
	[T32] [int] NOT NULL,
	[S33] [bigint] NOT NULL,
	[T33] [int] NOT NULL,
	[S34] [bigint] NOT NULL,
	[T34] [int] NOT NULL,
	[S35] [bigint] NOT NULL,
	[T35] [int] NOT NULL,
	[S36] [bigint] NOT NULL,
	[T36] [int] NOT NULL,
	[S37] [bigint] NOT NULL,
	[T37] [int] NOT NULL,
	[S38] [bigint] NOT NULL,
	[T38] [int] NOT NULL,
	[S39] [bigint] NOT NULL,
	[T39] [int] NOT NULL,
	[S40] [bigint] NOT NULL,
	[T40] [int] NOT NULL,
	[S41] [bigint] NOT NULL,
	[T41] [int] NOT NULL,
	[S42] [bigint] NOT NULL,
	[T42] [int] NOT NULL,
	[S43] [bigint] NOT NULL,
	[T43] [int] NOT NULL,
	[S44] [bigint] NOT NULL,
	[T44] [int] NOT NULL,
	[S45] [bigint] NOT NULL,
	[T45] [int] NOT NULL,
	[S46] [bigint] NOT NULL,
	[T46] [int] NOT NULL,
	[S47] [bigint] NOT NULL,
	[T47] [int] NOT NULL,
	[S48] [bigint] NOT NULL,
	[T48] [int] NOT NULL,
	[S49] [bigint] NOT NULL,
	[T49] [int] NOT NULL,
	[S50] [bigint] NOT NULL,
	[T50] [int] NOT NULL,
	[S51] [bigint] NOT NULL,
	[T51] [int] NOT NULL,
	[S52] [bigint] NOT NULL,
	[T52] [int] NOT NULL,
	[S53] [bigint] NOT NULL,
	[T53] [int] NOT NULL,
	[S54] [bigint] NOT NULL,
	[T54] [int] NOT NULL,
	[S55] [bigint] NOT NULL,
	[T55] [int] NOT NULL,
	[S56] [bigint] NOT NULL,
	[T56] [int] NOT NULL,
	[S57] [bigint] NOT NULL,
	[T57] [int] NOT NULL,
	[S58] [bigint] NOT NULL,
	[T58] [int] NOT NULL,
	[S59] [bigint] NOT NULL,
	[T59] [int] NOT NULL,
	[S60] [bigint] NOT NULL,
	[T60] [int] NOT NULL,
	[S61] [bigint] NOT NULL,
	[T61] [int] NOT NULL,
	[S62] [bigint] NOT NULL,
	[T62] [int] NOT NULL,
	[S63] [bigint] NOT NULL,
	[T63] [int] NOT NULL,
	[S64] [bigint] NOT NULL,
	[T64] [int] NOT NULL,
	[S65] [bigint] NOT NULL,
	[T65] [int] NOT NULL,
	[S66] [bigint] NOT NULL,
	[T66] [int] NOT NULL,
	[S67] [bigint] NOT NULL,
	[T67] [int] NOT NULL,
	[S68] [bigint] NOT NULL,
	[T68] [int] NOT NULL,
	[S69] [bigint] NOT NULL,
	[T69] [int] NOT NULL,
	[S70] [bigint] NOT NULL,
	[T70] [int] NOT NULL,
	[S71] [bigint] NOT NULL,
	[T71] [int] NOT NULL,
	[S72] [bigint] NOT NULL,
	[T72] [int] NOT NULL,
	[S73] [bigint] NOT NULL,
	[T73] [int] NOT NULL,
	[S74] [bigint] NOT NULL,
	[T74] [int] NOT NULL,
	[S75] [bigint] NOT NULL,
	[T75] [int] NOT NULL,
	[S76] [bigint] NOT NULL,
	[T76] [int] NOT NULL,
	[S77] [bigint] NOT NULL,
	[T77] [int] NOT NULL,
	[S78] [bigint] NOT NULL,
	[T78] [int] NOT NULL,
	[S79] [bigint] NOT NULL,
	[T79] [int] NOT NULL,
	[S80] [bigint] NOT NULL,
	[T80] [int] NOT NULL,
	[S81] [bigint] NOT NULL,
	[T81] [int] NOT NULL,
	[S82] [bigint] NOT NULL,
	[T82] [int] NOT NULL,
	[S83] [bigint] NOT NULL,
	[T83] [int] NOT NULL,
	[S84] [bigint] NOT NULL,
	[T84] [int] NOT NULL,
	[S85] [bigint] NOT NULL,
	[T85] [int] NOT NULL,
	[S86] [bigint] NOT NULL,
	[T86] [int] NOT NULL,
	[S87] [bigint] NOT NULL,
	[T87] [int] NOT NULL,
	[S88] [bigint] NOT NULL,
	[T88] [int] NOT NULL,
	[S89] [bigint] NOT NULL,
	[T89] [int] NOT NULL,
	[S90] [bigint] NOT NULL,
	[T90] [int] NOT NULL,
	[S91] [bigint] NOT NULL,
	[T91] [int] NOT NULL,
	[S92] [bigint] NOT NULL,
	[T92] [int] NOT NULL,
	[S93] [bigint] NOT NULL,
	[T93] [int] NOT NULL,
	[S94] [bigint] NOT NULL,
	[T94] [int] NOT NULL,
	[S95] [bigint] NOT NULL,
	[T95] [int] NOT NULL,
	[S96] [bigint] NOT NULL,
	[T96] [int] NOT NULL,
	[S97] [bigint] NOT NULL,
	[T97] [int] NOT NULL,
	[S98] [bigint] NOT NULL,
	[T98] [int] NOT NULL,
	[S99] [bigint] NOT NULL,
	[T99] [int] NOT NULL,
 CONSTRAINT [PK_tbl_AccountTrunk] PRIMARY KEY CLUSTERED 
(
	[AccountSerial] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[tbl_AccountTrunk_Extend]    Script Date: 12/01/2026 16:57:19 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[tbl_AccountTrunk_Extend](
	[AccountSerial] [int] NOT NULL,
	[DCK] [bit] NOT NULL,
	[EstSlot] [tinyint] NOT NULL,
	[K0] [int] NOT NULL,
	[D0] [bigint] NOT NULL,
	[U0] [int] NOT NULL,
	[R0] [tinyint] NOT NULL,
	[S0] [bigint] NOT NULL,
	[T0] [int] NOT NULL,
	[K1] [int] NOT NULL,
	[D1] [bigint] NOT NULL,
	[U1] [int] NOT NULL,
	[R1] [tinyint] NOT NULL,
	[S1] [bigint] NOT NULL,
	[T1] [int] NOT NULL,
	[K2] [int] NOT NULL,
	[D2] [bigint] NOT NULL,
	[U2] [int] NOT NULL,
	[R2] [tinyint] NOT NULL,
	[S2] [bigint] NOT NULL,
	[T2] [int] NOT NULL,
	[K3] [int] NOT NULL,
	[D3] [bigint] NOT NULL,
	[U3] [int] NOT NULL,
	[R3] [tinyint] NOT NULL,
	[S3] [bigint] NOT NULL,
	[T3] [int] NOT NULL,
	[K4] [int] NOT NULL,
	[D4] [bigint] NOT NULL,
	[U4] [int] NOT NULL,
	[R4] [tinyint] NOT NULL,
	[S4] [bigint] NOT NULL,
	[T4] [int] NOT NULL,
	[K5] [int] NOT NULL,
	[D5] [bigint] NOT NULL,
	[U5] [int] NOT NULL,
	[R5] [tinyint] NOT NULL,
	[S5] [bigint] NOT NULL,
	[T5] [int] NOT NULL,
	[K6] [int] NOT NULL,
	[D6] [bigint] NOT NULL,
	[U6] [int] NOT NULL,
	[R6] [tinyint] NOT NULL,
	[S6] [bigint] NOT NULL,
	[T6] [int] NOT NULL,
	[K7] [int] NOT NULL,
	[D7] [bigint] NOT NULL,
	[U7] [int] NOT NULL,
	[R7] [tinyint] NOT NULL,
	[S7] [bigint] NOT NULL,
	[T7] [int] NOT NULL,
	[K8] [int] NOT NULL,
	[D8] [bigint] NOT NULL,
	[U8] [int] NOT NULL,
	[R8] [tinyint] NOT NULL,
	[S8] [bigint] NOT NULL,
	[T8] [int] NOT NULL,
	[K9] [int] NOT NULL,
	[D9] [bigint] NOT NULL,
	[U9] [int] NOT NULL,
	[R9] [tinyint] NOT NULL,
	[S9] [bigint] NOT NULL,
	[T9] [int] NOT NULL,
	[K10] [int] NOT NULL,
	[D10] [bigint] NOT NULL,
	[U10] [int] NOT NULL,
	[R10] [tinyint] NOT NULL,
	[S10] [bigint] NOT NULL,
	[T10] [int] NOT NULL,
	[K11] [int] NOT NULL,
	[D11] [bigint] NOT NULL,
	[U11] [int] NOT NULL,
	[R11] [tinyint] NOT NULL,
	[S11] [bigint] NOT NULL,
	[T11] [int] NOT NULL,
	[K12] [int] NOT NULL,
	[D12] [bigint] NOT NULL,
	[U12] [int] NOT NULL,
	[R12] [tinyint] NOT NULL,
	[S12] [bigint] NOT NULL,
	[T12] [int] NOT NULL,
	[K13] [int] NOT NULL,
	[D13] [bigint] NOT NULL,
	[U13] [int] NOT NULL,
	[R13] [tinyint] NOT NULL,
	[S13] [bigint] NOT NULL,
	[T13] [int] NOT NULL,
	[K14] [int] NOT NULL,
	[D14] [bigint] NOT NULL,
	[U14] [int] NOT NULL,
	[R14] [tinyint] NOT NULL,
	[S14] [bigint] NOT NULL,
	[T14] [int] NOT NULL,
	[K15] [int] NOT NULL,
	[D15] [bigint] NOT NULL,
	[U15] [int] NOT NULL,
	[R15] [tinyint] NOT NULL,
	[S15] [bigint] NOT NULL,
	[T15] [int] NOT NULL,
	[K16] [int] NOT NULL,
	[D16] [bigint] NOT NULL,
	[U16] [int] NOT NULL,
	[R16] [tinyint] NOT NULL,
	[S16] [bigint] NOT NULL,
	[T16] [int] NOT NULL,
	[K17] [int] NOT NULL,
	[D17] [bigint] NOT NULL,
	[U17] [int] NOT NULL,
	[R17] [tinyint] NOT NULL,
	[S17] [bigint] NOT NULL,
	[T17] [int] NOT NULL,
	[K18] [int] NOT NULL,
	[D18] [bigint] NOT NULL,
	[U18] [int] NOT NULL,
	[R18] [tinyint] NOT NULL,
	[S18] [bigint] NOT NULL,
	[T18] [int] NOT NULL,
	[K19] [int] NOT NULL,
	[D19] [bigint] NOT NULL,
	[U19] [int] NOT NULL,
	[R19] [tinyint] NOT NULL,
	[S19] [bigint] NOT NULL,
	[T19] [int] NOT NULL,
	[K20] [int] NOT NULL,
	[D20] [bigint] NOT NULL,
	[U20] [int] NOT NULL,
	[R20] [tinyint] NOT NULL,
	[S20] [bigint] NOT NULL,
	[T20] [int] NOT NULL,
	[K21] [int] NOT NULL,
	[D21] [bigint] NOT NULL,
	[U21] [int] NOT NULL,
	[R21] [tinyint] NOT NULL,
	[S21] [bigint] NOT NULL,
	[T21] [int] NOT NULL,
	[K22] [int] NOT NULL,
	[D22] [bigint] NOT NULL,
	[U22] [int] NOT NULL,
	[R22] [tinyint] NOT NULL,
	[S22] [bigint] NOT NULL,
	[T22] [int] NOT NULL,
	[K23] [int] NOT NULL,
	[D23] [bigint] NOT NULL,
	[U23] [int] NOT NULL,
	[R23] [tinyint] NOT NULL,
	[S23] [bigint] NOT NULL,
	[T23] [int] NOT NULL,
	[K24] [int] NOT NULL,
	[D24] [bigint] NOT NULL,
	[U24] [int] NOT NULL,
	[R24] [tinyint] NOT NULL,
	[S24] [bigint] NOT NULL,
	[T24] [int] NOT NULL,
	[K25] [int] NOT NULL,
	[D25] [bigint] NOT NULL,
	[U25] [int] NOT NULL,
	[R25] [tinyint] NOT NULL,
	[S25] [bigint] NOT NULL,
	[T25] [int] NOT NULL,
	[K26] [int] NOT NULL,
	[D26] [bigint] NOT NULL,
	[U26] [int] NOT NULL,
	[R26] [tinyint] NOT NULL,
	[S26] [bigint] NOT NULL,
	[T26] [int] NOT NULL,
	[K27] [int] NOT NULL,
	[D27] [bigint] NOT NULL,
	[U27] [int] NOT NULL,
	[R27] [tinyint] NOT NULL,
	[S27] [bigint] NOT NULL,
	[T27] [int] NOT NULL,
	[K28] [int] NOT NULL,
	[D28] [bigint] NOT NULL,
	[U28] [int] NOT NULL,
	[R28] [tinyint] NOT NULL,
	[S28] [bigint] NOT NULL,
	[T28] [int] NOT NULL,
	[K29] [int] NOT NULL,
	[D29] [bigint] NOT NULL,
	[U29] [int] NOT NULL,
	[R29] [tinyint] NOT NULL,
	[S29] [bigint] NOT NULL,
	[T29] [int] NOT NULL,
	[K30] [int] NOT NULL,
	[D30] [bigint] NOT NULL,
	[U30] [int] NOT NULL,
	[R30] [tinyint] NOT NULL,
	[S30] [bigint] NOT NULL,
	[T30] [int] NOT NULL,
	[K31] [int] NOT NULL,
	[D31] [bigint] NOT NULL,
	[U31] [int] NOT NULL,
	[R31] [tinyint] NOT NULL,
	[S31] [bigint] NOT NULL,
	[T31] [int] NOT NULL,
	[K32] [int] NOT NULL,
	[D32] [bigint] NOT NULL,
	[U32] [int] NOT NULL,
	[R32] [tinyint] NOT NULL,
	[S32] [bigint] NOT NULL,
	[T32] [int] NOT NULL,
	[K33] [int] NOT NULL,
	[D33] [bigint] NOT NULL,
	[U33] [int] NOT NULL,
	[R33] [tinyint] NOT NULL,
	[S33] [bigint] NOT NULL,
	[T33] [int] NOT NULL,
	[K34] [int] NOT NULL,
	[D34] [bigint] NOT NULL,
	[U34] [int] NOT NULL,
	[R34] [tinyint] NOT NULL,
	[S34] [bigint] NOT NULL,
	[T34] [int] NOT NULL,
	[K35] [int] NOT NULL,
	[D35] [bigint] NOT NULL,
	[U35] [int] NOT NULL,
	[R35] [tinyint] NOT NULL,
	[S35] [bigint] NOT NULL,
	[T35] [int] NOT NULL,
	[K36] [int] NOT NULL,
	[D36] [bigint] NOT NULL,
	[U36] [int] NOT NULL,
	[R36] [tinyint] NOT NULL,
	[S36] [bigint] NOT NULL,
	[T36] [int] NOT NULL,
	[K37] [int] NOT NULL,
	[D37] [bigint] NOT NULL,
	[U37] [int] NOT NULL,
	[R37] [tinyint] NOT NULL,
	[S37] [bigint] NOT NULL,
	[T37] [int] NOT NULL,
	[K38] [int] NOT NULL,
	[D38] [bigint] NOT NULL,
	[U38] [int] NOT NULL,
	[R38] [tinyint] NOT NULL,
	[S38] [bigint] NOT NULL,
	[T38] [int] NOT NULL,
	[K39] [int] NOT NULL,
	[D39] [bigint] NOT NULL,
	[U39] [int] NOT NULL,
	[R39] [tinyint] NOT NULL,
	[S39] [bigint] NOT NULL,
	[T39] [int] NOT NULL,
 CONSTRAINT [PK_tbl_AccountTrunk_Extend] PRIMARY KEY CLUSTERED 
(
	[AccountSerial] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[tbl_AccountTrunkCharge]    Script Date: 12/01/2026 16:57:19 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[tbl_AccountTrunkCharge](
	[ID] [int] IDENTITY(1,1) NOT NULL,
	[AccountSerial] [int] NOT NULL,
	[DCK] [bit] NOT NULL,
	[TID] [tinyint] NOT NULL,
	[Money] [float] NOT NULL,
	[K] [int] NOT NULL,
	[D] [bigint] NOT NULL,
	[U] [int] NOT NULL,
	[R] [int] NOT NULL,
	[GiveDate] [datetime] NOT NULL,
	[TakeDate] [datetime] NOT NULL,
	[S] [bigint] NOT NULL,
	[T] [int] NOT NULL,
 CONSTRAINT [PK_tbl_AccountTrunkCharge] PRIMARY KEY CLUSTERED 
(
	[ID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[tbl_aminepersonal_inven]    Script Date: 12/01/2026 16:57:19 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[tbl_aminepersonal_inven](
	[serial] [int] IDENTITY(1,1) NOT NULL,
	[avatorserial] [int] NOT NULL,
	[dck] [bit] NOT NULL,
	[tmReg] [datetime] NOT NULL,
	[tmUpdate] [datetime] NOT NULL,
	[K0] [int] NOT NULL,
	[N0] [tinyint] NOT NULL,
	[K1] [int] NOT NULL,
	[N1] [tinyint] NOT NULL,
	[K2] [int] NOT NULL,
	[N2] [tinyint] NOT NULL,
	[K3] [int] NOT NULL,
	[N3] [tinyint] NOT NULL,
	[K4] [int] NOT NULL,
	[N4] [tinyint] NOT NULL,
	[K5] [int] NOT NULL,
	[N5] [tinyint] NOT NULL,
	[K6] [int] NOT NULL,
	[N6] [tinyint] NOT NULL,
	[K7] [int] NOT NULL,
	[N7] [tinyint] NOT NULL,
	[K8] [int] NOT NULL,
	[N8] [tinyint] NOT NULL,
	[K9] [int] NOT NULL,
	[N9] [tinyint] NOT NULL,
	[K10] [int] NOT NULL,
	[N10] [tinyint] NOT NULL,
	[K11] [int] NOT NULL,
	[N11] [tinyint] NOT NULL,
	[K12] [int] NOT NULL,
	[N12] [tinyint] NOT NULL,
	[K13] [int] NOT NULL,
	[N13] [tinyint] NOT NULL,
	[K14] [int] NOT NULL,
	[N14] [tinyint] NOT NULL,
	[K15] [int] NOT NULL,
	[N15] [tinyint] NOT NULL,
	[K16] [int] NOT NULL,
	[N16] [tinyint] NOT NULL,
	[K17] [int] NOT NULL,
	[N17] [tinyint] NOT NULL,
	[K18] [int] NOT NULL,
	[N18] [tinyint] NOT NULL,
	[K19] [int] NOT NULL,
	[N19] [tinyint] NOT NULL,
	[K20] [int] NOT NULL,
	[N20] [tinyint] NOT NULL,
	[K21] [int] NOT NULL,
	[N21] [tinyint] NOT NULL,
	[K22] [int] NOT NULL,
	[N22] [tinyint] NOT NULL,
	[K23] [int] NOT NULL,
	[N23] [tinyint] NOT NULL,
	[K24] [int] NOT NULL,
	[N24] [tinyint] NOT NULL,
	[K25] [int] NOT NULL,
	[N25] [tinyint] NOT NULL,
	[K26] [int] NOT NULL,
	[N26] [tinyint] NOT NULL,
	[K27] [int] NOT NULL,
	[N27] [tinyint] NOT NULL,
	[K28] [int] NOT NULL,
	[N28] [tinyint] NOT NULL,
	[K29] [int] NOT NULL,
	[N29] [tinyint] NOT NULL,
	[K30] [int] NOT NULL,
	[N30] [tinyint] NOT NULL,
	[K31] [int] NOT NULL,
	[N31] [tinyint] NOT NULL,
	[K32] [int] NOT NULL,
	[N32] [tinyint] NOT NULL,
	[K33] [int] NOT NULL,
	[N33] [tinyint] NOT NULL,
	[K34] [int] NOT NULL,
	[N34] [tinyint] NOT NULL,
	[K35] [int] NOT NULL,
	[N35] [tinyint] NOT NULL,
	[K36] [int] NOT NULL,
	[N36] [tinyint] NOT NULL,
	[K37] [int] NOT NULL,
	[N37] [tinyint] NOT NULL,
	[K38] [int] NOT NULL,
	[N38] [tinyint] NOT NULL,
	[K39] [int] NOT NULL,
	[N39] [tinyint] NOT NULL,
 CONSTRAINT [PK_tbl_aminepersonal_inven] PRIMARY KEY CLUSTERED 
(
	[serial] ASC,
	[avatorserial] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY],
 CONSTRAINT [DF_tbl_animepersonal_inven_avatorserial] UNIQUE NONCLUSTERED 
(
	[avatorserial] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[tbl_animusdata]    Script Date: 12/01/2026 16:57:19 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[tbl_animusdata](
	[Serial] [int] NOT NULL,
	[Data0] [float] NOT NULL,
	[Data1] [float] NOT NULL,
	[Data2] [float] NOT NULL,
	[Data3] [float] NOT NULL,
	[Data4] [float] NOT NULL,
	[Data5] [float] NOT NULL,
 CONSTRAINT [PK_tbl_animusdata] PRIMARY KEY CLUSTERED 
(
	[Serial] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[tbl_animuslog]    Script Date: 12/01/2026 16:57:19 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[tbl_animuslog](
	[ID] [int] IDENTITY(1,1) NOT NULL,
	[Serial] [int] NOT NULL,
	[Name] [varchar](17) NOT NULL,
	[DID] [tinyint] NOT NULL,
	[OrgValue] [float] NOT NULL,
	[ChangeValue] [float] NOT NULL,
	[LogDate] [datetime] NOT NULL,
 CONSTRAINT [PK_tbl_animuslog] PRIMARY KEY CLUSTERED 
(
	[ID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[tbl_ATradeTaxRate]    Script Date: 12/01/2026 16:57:19 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[tbl_ATradeTaxRate](
	[serial] [int] IDENTITY(1,1) NOT NULL,
	[Race] [tinyint] NOT NULL,
	[GSerial] [int] NOT NULL,
	[GName] [nvarchar](24) NOT NULL,
	[Tax] [tinyint] NOT NULL,
	[NextTax] [tinyint] NOT NULL,
	[UpdateTime] [datetime] NOT NULL,
	[Suggester] [int] NOT NULL,
	[SuggesterName] [nvarchar](32) NOT NULL,
 CONSTRAINT [PK_tbl_ATradeTaxRate] PRIMARY KEY CLUSTERED 
(
	[serial] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[tbl_automine_inven]    Script Date: 12/01/2026 16:57:19 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[tbl_automine_inven](
	[serial] [int] IDENTITY(1,1) NOT NULL,
	[dck] [bit] NOT NULL,
	[Race] [tinyint] NOT NULL,
	[CollisionType] [tinyint] NOT NULL,
	[GuildSerial] [int] NOT NULL,
	[bWorking] [bit] NOT NULL,
	[SelectOre] [tinyint] NOT NULL,
	[Battery] [int] NOT NULL,
	[tmReg] [datetime] NOT NULL,
	[tmLastUpdate] [datetime] NOT NULL,
	[K_0] [int] NOT NULL,
	[O_0] [tinyint] NOT NULL,
	[K_1] [int] NOT NULL,
	[O_1] [tinyint] NOT NULL,
	[K_2] [int] NOT NULL,
	[O_2] [tinyint] NOT NULL,
	[K_3] [int] NOT NULL,
	[O_3] [tinyint] NOT NULL,
	[K_4] [int] NOT NULL,
	[O_4] [tinyint] NOT NULL,
	[K_5] [int] NOT NULL,
	[O_5] [tinyint] NOT NULL,
	[K_6] [int] NOT NULL,
	[O_6] [tinyint] NOT NULL,
	[K_7] [int] NOT NULL,
	[O_7] [tinyint] NOT NULL,
	[K_8] [int] NOT NULL,
	[O_8] [tinyint] NOT NULL,
	[K_9] [int] NOT NULL,
	[O_9] [tinyint] NOT NULL,
	[K_10] [int] NOT NULL,
	[O_10] [tinyint] NOT NULL,
	[K_11] [int] NOT NULL,
	[O_11] [tinyint] NOT NULL,
	[K_12] [int] NOT NULL,
	[O_12] [tinyint] NOT NULL,
	[K_13] [int] NOT NULL,
	[O_13] [tinyint] NOT NULL,
	[K_14] [int] NOT NULL,
	[O_14] [tinyint] NOT NULL,
	[K_15] [int] NOT NULL,
	[O_15] [tinyint] NOT NULL,
	[K_16] [int] NOT NULL,
	[O_16] [tinyint] NOT NULL,
	[K_17] [int] NOT NULL,
	[O_17] [tinyint] NOT NULL,
	[K_18] [int] NOT NULL,
	[O_18] [tinyint] NOT NULL,
	[K_19] [int] NOT NULL,
	[O_19] [tinyint] NOT NULL,
	[K_20] [int] NOT NULL,
	[O_20] [tinyint] NOT NULL,
	[K_21] [int] NOT NULL,
	[O_21] [tinyint] NOT NULL,
	[K_22] [int] NOT NULL,
	[O_22] [tinyint] NOT NULL,
	[K_23] [int] NOT NULL,
	[O_23] [tinyint] NOT NULL,
	[K_24] [int] NOT NULL,
	[O_24] [tinyint] NOT NULL,
	[K_25] [int] NOT NULL,
	[O_25] [tinyint] NOT NULL,
	[K_26] [int] NOT NULL,
	[O_26] [tinyint] NOT NULL,
	[K_27] [int] NOT NULL,
	[O_27] [tinyint] NOT NULL,
	[K_28] [int] NOT NULL,
	[O_28] [tinyint] NOT NULL,
	[K_29] [int] NOT NULL,
	[O_29] [tinyint] NOT NULL,
	[K_30] [int] NOT NULL,
	[O_30] [tinyint] NOT NULL,
	[K_31] [int] NOT NULL,
	[O_31] [tinyint] NOT NULL,
	[K_32] [int] NOT NULL,
	[O_32] [tinyint] NOT NULL,
	[K_33] [int] NOT NULL,
	[O_33] [tinyint] NOT NULL,
	[K_34] [int] NOT NULL,
	[O_34] [tinyint] NOT NULL,
	[K_35] [int] NOT NULL,
	[O_35] [tinyint] NOT NULL,
	[K_36] [int] NOT NULL,
	[O_36] [tinyint] NOT NULL,
	[K_37] [int] NOT NULL,
	[O_37] [tinyint] NOT NULL,
	[K_38] [int] NOT NULL,
	[O_38] [tinyint] NOT NULL,
	[K_39] [int] NOT NULL,
	[O_39] [tinyint] NOT NULL,
	[K_40] [int] NOT NULL,
	[O_40] [tinyint] NOT NULL,
	[K_41] [int] NOT NULL,
	[O_41] [tinyint] NOT NULL,
	[K_42] [int] NOT NULL,
	[O_42] [tinyint] NOT NULL,
	[K_43] [int] NOT NULL,
	[O_43] [tinyint] NOT NULL,
	[K_44] [int] NOT NULL,
	[O_44] [tinyint] NOT NULL,
	[K_45] [int] NOT NULL,
	[O_45] [tinyint] NOT NULL,
	[K_46] [int] NOT NULL,
	[O_46] [tinyint] NOT NULL,
	[K_47] [int] NOT NULL,
	[O_47] [tinyint] NOT NULL,
	[K_48] [int] NOT NULL,
	[O_48] [tinyint] NOT NULL,
	[K_49] [int] NOT NULL,
	[O_49] [tinyint] NOT NULL,
	[K_50] [int] NOT NULL,
	[O_50] [tinyint] NOT NULL,
	[K_51] [int] NOT NULL,
	[O_51] [tinyint] NOT NULL,
	[K_52] [int] NOT NULL,
	[O_52] [tinyint] NOT NULL,
	[K_53] [int] NOT NULL,
	[O_53] [tinyint] NOT NULL,
	[K_54] [int] NOT NULL,
	[O_54] [tinyint] NOT NULL,
	[K_55] [int] NOT NULL,
	[O_55] [tinyint] NOT NULL,
	[K_56] [int] NOT NULL,
	[O_56] [tinyint] NOT NULL,
	[K_57] [int] NOT NULL,
	[O_57] [tinyint] NOT NULL,
	[K_58] [int] NOT NULL,
	[O_58] [tinyint] NOT NULL,
	[K_59] [int] NOT NULL,
	[O_59] [tinyint] NOT NULL,
	[K_60] [int] NOT NULL,
	[O_60] [tinyint] NOT NULL,
	[K_61] [int] NOT NULL,
	[O_61] [tinyint] NOT NULL,
	[K_62] [int] NOT NULL,
	[O_62] [tinyint] NOT NULL,
	[K_63] [int] NOT NULL,
	[O_63] [tinyint] NOT NULL,
	[K_64] [int] NOT NULL,
	[O_64] [tinyint] NOT NULL,
	[K_65] [int] NOT NULL,
	[O_65] [tinyint] NOT NULL,
	[K_66] [int] NOT NULL,
	[O_66] [tinyint] NOT NULL,
	[K_67] [int] NOT NULL,
	[O_67] [tinyint] NOT NULL,
	[K_68] [int] NOT NULL,
	[O_68] [tinyint] NOT NULL,
	[K_69] [int] NOT NULL,
	[O_69] [tinyint] NOT NULL,
	[K_70] [int] NOT NULL,
	[O_70] [tinyint] NOT NULL,
	[K_71] [int] NOT NULL,
	[O_71] [tinyint] NOT NULL,
	[K_72] [int] NOT NULL,
	[O_72] [tinyint] NOT NULL,
	[K_73] [int] NOT NULL,
	[O_73] [tinyint] NOT NULL,
	[K_74] [int] NOT NULL,
	[O_74] [tinyint] NOT NULL,
	[K_75] [int] NOT NULL,
	[O_75] [tinyint] NOT NULL,
	[K_76] [int] NOT NULL,
	[O_76] [tinyint] NOT NULL,
	[K_77] [int] NOT NULL,
	[O_77] [tinyint] NOT NULL,
	[K_78] [int] NOT NULL,
	[O_78] [tinyint] NOT NULL,
	[K_79] [int] NOT NULL,
	[O_79] [tinyint] NOT NULL,
 CONSTRAINT [PK_tbl_automine_inven] PRIMARY KEY CLUSTERED 
(
	[serial] ASC,
	[dck] ASC,
	[Race] ASC,
	[CollisionType] ASC,
	[GuildSerial] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[tbl_base]    Script Date: 12/01/2026 16:57:19 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[tbl_base](
	[Serial] [int] IDENTITY(1,1) NOT NULL,
	[DCK] [bit] NOT NULL,
	[Lock] [int] NOT NULL,
	[Name] [nvarchar](17) NOT NULL,
	[AccountSerial] [int] NOT NULL,
	[Account] [varchar](17) NOT NULL,
	[Slot] [int] NOT NULL,
	[Race] [int] NOT NULL,
	[Class] [char](4) NOT NULL,
	[Lv] [int] NOT NULL,
	[Dalant] [int] NOT NULL,
	[Dalant64] [bigint] NOT NULL,
	[Gold] [int] NOT NULL,
	[Gold64] [bigint] NOT NULL,
	[BaseShape] [int] NOT NULL,
	[EK0] [int] NOT NULL,
	[EU0] [int] NOT NULL,
	[EK1] [int] NOT NULL,
	[EU1] [int] NOT NULL,
	[EK2] [int] NOT NULL,
	[EU2] [int] NOT NULL,
	[EK3] [int] NOT NULL,
	[EU3] [int] NOT NULL,
	[EK4] [int] NOT NULL,
	[EU4] [int] NOT NULL,
	[EK5] [int] NOT NULL,
	[EU5] [int] NOT NULL,
	[EK6] [int] NOT NULL,
	[EU6] [int] NOT NULL,
	[EK7] [int] NOT NULL,
	[EU7] [int] NOT NULL,
	[LastConnTime] [bigint] NOT NULL,
	[CreateTime] [datetime] NOT NULL,
	[DeleteTime] [datetime] NOT NULL,
	[DeleteName] [nvarchar](17) NULL,
	[FirstConnTime] [int] NULL,
	[HomeServer] [varchar](33) NOT NULL,
	[Arrange] [tinyint] NOT NULL,
	[ES0] [bigint] NOT NULL,
	[ET0] [int] NOT NULL,
	[ES1] [bigint] NOT NULL,
	[ET1] [int] NOT NULL,
	[ES2] [bigint] NOT NULL,
	[ET2] [int] NOT NULL,
	[ES3] [bigint] NOT NULL,
	[ET3] [int] NOT NULL,
	[ES4] [bigint] NOT NULL,
	[ET4] [int] NOT NULL,
	[ES5] [bigint] NOT NULL,
	[ET5] [int] NOT NULL,
	[ES6] [bigint] NOT NULL,
	[ET6] [int] NOT NULL,
	[ES7] [bigint] NOT NULL,
	[ET7] [int] NOT NULL,
	[EK8] [int] NOT NULL,
	[EU8] [int] NOT NULL,
	[ES8] [bigint] NOT NULL,
	[ET8] [int] NOT NULL,
 CONSTRAINT [PK_tbl_base] PRIMARY KEY CLUSTERED 
(
	[Serial] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY],
 CONSTRAINT [IX_tbl_base] UNIQUE NONCLUSTERED 
(
	[Name] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[tbl_battletournament]    Script Date: 12/01/2026 16:57:19 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[tbl_battletournament](
	[CharacterSerial] [int] NOT NULL,
	[CharacterName] [varchar](17) NOT NULL,
	[BattleWinGrade] [tinyint] NOT NULL,
 CONSTRAINT [PK__tbl_battletournament] PRIMARY KEY CLUSTERED 
(
	[CharacterSerial] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[tbl_Buddy]    Script Date: 12/01/2026 16:57:19 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[tbl_Buddy](
	[Serial] [int] NOT NULL,
	[Serial0] [int] NOT NULL,
	[Serial1] [int] NOT NULL,
	[Serial2] [int] NOT NULL,
	[Serial3] [int] NOT NULL,
	[Serial4] [int] NOT NULL,
	[Serial5] [int] NOT NULL,
	[Serial6] [int] NOT NULL,
	[Serial7] [int] NOT NULL,
	[Serial8] [int] NOT NULL,
	[Serial9] [int] NOT NULL,
	[Serial10] [int] NOT NULL,
	[Serial11] [int] NOT NULL,
	[Serial12] [int] NOT NULL,
	[Serial13] [int] NOT NULL,
	[Serial14] [int] NOT NULL,
	[Serial15] [int] NOT NULL,
	[Serial16] [int] NOT NULL,
	[Serial17] [int] NOT NULL,
	[Serial18] [int] NOT NULL,
	[Serial19] [int] NOT NULL,
	[Serial20] [int] NOT NULL,
	[Serial21] [int] NOT NULL,
	[Serial22] [int] NOT NULL,
	[Serial23] [int] NOT NULL,
	[Serial24] [int] NOT NULL,
	[Serial25] [int] NOT NULL,
	[Serial26] [int] NOT NULL,
	[Serial27] [int] NOT NULL,
	[Serial28] [int] NOT NULL,
	[Serial29] [int] NOT NULL,
	[Serial30] [int] NOT NULL,
	[Serial31] [int] NOT NULL,
	[Serial32] [int] NOT NULL,
	[Serial33] [int] NOT NULL,
	[Serial34] [int] NOT NULL,
	[Serial35] [int] NOT NULL,
	[Serial36] [int] NOT NULL,
	[Serial37] [int] NOT NULL,
	[Serial38] [int] NOT NULL,
	[Serial39] [int] NOT NULL,
	[Serial40] [int] NOT NULL,
	[Serial41] [int] NOT NULL,
	[Serial42] [int] NOT NULL,
	[Serial43] [int] NOT NULL,
	[Serial44] [int] NOT NULL,
	[Serial45] [int] NOT NULL,
	[Serial46] [int] NOT NULL,
	[Serial47] [int] NOT NULL,
	[Serial48] [int] NOT NULL,
	[Serial49] [int] NOT NULL,
	[DCK] [bit] NOT NULL,
 CONSTRAINT [PK_tbl_Buddy] PRIMARY KEY CLUSTERED 
(
	[Serial] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[tbl_Cash_LimSale]    Script Date: 12/01/2026 16:57:19 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[tbl_Cash_LimSale](
	[index] [int] IDENTITY(1,1) NOT NULL,
	[DCK] [tinyint] NOT NULL,
	[LimSaleNum] [tinyint] NOT NULL,
	[Code_K0] [int] NOT NULL,
	[Num0] [int] NOT NULL,
	[Code_K1] [int] NOT NULL,
	[Num1] [int] NOT NULL,
	[Code_K2] [int] NOT NULL,
	[Num2] [int] NOT NULL,
	[Code_K3] [int] NOT NULL,
	[Num3] [int] NOT NULL,
	[Code_K4] [int] NOT NULL,
	[Num4] [int] NOT NULL,
	[Code_K5] [int] NOT NULL,
	[Num5] [int] NOT NULL,
	[Code_K6] [int] NOT NULL,
	[Num6] [int] NOT NULL,
	[Code_K7] [int] NOT NULL,
	[Num7] [int] NOT NULL,
	[Code_K8] [int] NOT NULL,
	[Num8] [int] NOT NULL,
	[Code_K9] [int] NOT NULL,
	[Num9] [int] NOT NULL,
	[Code_K10] [int] NOT NULL,
	[Num10] [int] NOT NULL,
	[Code_K11] [int] NOT NULL,
	[Num11] [int] NOT NULL,
	[Code_K12] [int] NOT NULL,
	[Num12] [int] NOT NULL,
	[Code_K13] [int] NOT NULL,
	[Num13] [int] NOT NULL,
	[Code_K14] [int] NOT NULL,
	[Num14] [int] NOT NULL,
	[Code_K15] [int] NOT NULL,
	[Num15] [int] NOT NULL,
	[Code_K16] [int] NOT NULL,
	[Num16] [int] NOT NULL,
	[Code_K17] [int] NOT NULL,
	[Num17] [int] NOT NULL,
	[Code_K18] [int] NOT NULL,
	[Num18] [int] NOT NULL,
	[Code_K19] [int] NOT NULL,
	[Num19] [int] NOT NULL,
 CONSTRAINT [PK_tbl_Cash_LimSale] PRIMARY KEY CLUSTERED 
(
	[index] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[tbl_characterselect_log_202410]    Script Date: 12/01/2026 16:57:19 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[tbl_characterselect_log_202410](
	[ID] [int] IDENTITY(1,1) NOT NULL,
	[AccountSerial] [int] NOT NULL,
	[Account] [varchar](17) NOT NULL,
	[CharacSerial] [int] NOT NULL,
	[CharacName] [varchar](17) NOT NULL,
	[LogDate] [datetime] NOT NULL,
 CONSTRAINT [PK_tbl_characterselect_log_202410] PRIMARY KEY CLUSTERED 
(
	[ID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[tbl_characterselect_log_202601]    Script Date: 12/01/2026 16:57:19 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[tbl_characterselect_log_202601](
	[ID] [int] IDENTITY(1,1) NOT NULL,
	[AccountSerial] [int] NOT NULL,
	[Account] [varchar](17) NOT NULL,
	[CharacSerial] [int] NOT NULL,
	[CharacName] [varchar](17) NOT NULL,
	[LogDate] [datetime] NOT NULL,
 CONSTRAINT [PK_tbl_characterselect_log_202601] PRIMARY KEY CLUSTERED 
(
	[ID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[tbl_ClassLog_AfterInitClass]    Script Date: 12/01/2026 16:57:19 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[tbl_ClassLog_AfterInitClass](
	[ID] [int] IDENTITY(1,1) NOT NULL,
	[Serial] [int] NOT NULL,
	[Type] [bit] NOT NULL,
	[PrevClass] [char](4) NOT NULL,
	[NextClass] [char](4) NOT NULL,
	[ClassInitCnt] [int] NOT NULL,
	[LastClassGrade] [tinyint] NOT NULL,
	[LogDate] [datetime] NOT NULL,
 CONSTRAINT [PK_tbl_Class_Log] PRIMARY KEY CLUSTERED 
(
	[ID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[tbl_CryMsg]    Script Date: 12/01/2026 16:57:19 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[tbl_CryMsg](
	[Serial] [int] NOT NULL,
	[CryMsg1] [varchar](255) NOT NULL,
	[CryMsg2] [varchar](255) NOT NULL,
	[CryMsg3] [varchar](255) NOT NULL,
	[CryMsg4] [varchar](255) NOT NULL,
	[CryMsg5] [varchar](255) NOT NULL,
	[CryMsg6] [varchar](255) NOT NULL,
	[CryMsg7] [varchar](255) NOT NULL,
	[CryMsg8] [varchar](255) NOT NULL,
	[CryMsg9] [varchar](255) NOT NULL,
	[CryMsg10] [varchar](255) NOT NULL,
 CONSTRAINT [PK_tbl_CryMsg] PRIMARY KEY CLUSTERED 
(
	[Serial] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[tbl_dormantaccount]    Script Date: 12/01/2026 16:57:19 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[tbl_dormantaccount](
	[AccountSerial] [int] NOT NULL,
	[Account] [varchar](17) NULL,
	[LastConnTime] [int] NULL,
 CONSTRAINT [PK_tbl_dormantaccount] PRIMARY KEY CLUSTERED 
(
	[AccountSerial] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[tbl_economy_history]    Script Date: 12/01/2026 16:57:19 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[tbl_economy_history](
	[Serial] [int] NOT NULL,
	[Updatedate] [datetime] NOT NULL,
	[B_Dalant] [float] NOT NULL,
	[B_Gold] [float] NOT NULL,
	[C_Dalant] [float] NOT NULL,
	[C_Gold] [float] NOT NULL,
	[A_Dalant] [float] NOT NULL,
	[A_Gold] [float] NOT NULL,
	[ManageValue] [int] NOT NULL,
	[B_MineOre1] [float] NULL,
	[B_MineOre2] [float] NULL,
	[B_MineOre3] [float] NULL,
	[C_MineOre1] [float] NULL,
	[C_MineOre2] [float] NULL,
	[C_MineOre3] [float] NULL,
	[A_MineOre1] [float] NULL,
	[A_MineOre2] [float] NULL,
	[A_MineOre3] [float] NULL,
	[B_CutOre1] [float] NULL,
	[B_CutOre2] [float] NULL,
	[B_CutOre3] [float] NULL,
	[C_CutOre1] [float] NULL,
	[C_CutOre2] [float] NULL,
	[C_CutOre3] [float] NULL,
	[A_CutOre1] [float] NULL,
	[A_CutOre2] [float] NULL,
	[A_CutOre3] [float] NULL,
 CONSTRAINT [PK_tbl_economy_history] PRIMARY KEY CLUSTERED 
(
	[Serial] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[tbl_event]    Script Date: 12/01/2026 16:57:19 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[tbl_event](
	[serial] [int] IDENTITY(1,1) NOT NULL,
	[AvatorSerial] [int] NOT NULL,
	[ClassRefineCnt] [tinyint] NOT NULL,
	[ClassRefineDate] [int] NOT NULL,
 CONSTRAINT [PK_tbl_event] PRIMARY KEY CLUSTERED 
(
	[serial] ASC,
	[AvatorSerial] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[tbl_EventPoint]    Script Date: 12/01/2026 16:57:19 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[tbl_EventPoint](
	[AccountSerial] [int] NOT NULL,
	[DCK] [bit] NOT NULL,
	[Account] [varchar](17) NOT NULL,
	[Point] [int] NOT NULL,
 CONSTRAINT [PK_tbl_EventPoint] PRIMARY KEY CLUSTERED 
(
	[AccountSerial] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[tbl_general]    Script Date: 12/01/2026 16:57:19 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[tbl_general](
	[Serial] [int] NOT NULL,
	[DCK] [bit] NOT NULL,
	[TotalPlayMin] [int] NOT NULL,
	[Exp] [float] NOT NULL,
	[LossExp] [float] NOT NULL,
	[HP] [int] NOT NULL,
	[FP] [int] NOT NULL,
	[SP] [int] NOT NULL,
	[DP] [int] NOT NULL,
	[Map] [int] NOT NULL,
	[X] [float] NOT NULL,
	[Y] [float] NOT NULL,
	[Z] [float] NOT NULL,
	[BagNum] [int] NOT NULL,
	[EK0] [int] NOT NULL,
	[ED0] [smallint] NOT NULL,
	[EK1] [int] NOT NULL,
	[ED1] [smallint] NOT NULL,
	[EK2] [int] NOT NULL,
	[ED2] [smallint] NOT NULL,
	[EK3] [int] NOT NULL,
	[ED3] [smallint] NOT NULL,
	[EK4] [int] NOT NULL,
	[ED4] [smallint] NOT NULL,
	[EK5] [int] NOT NULL,
	[ED5] [smallint] NOT NULL,
	[F0] [int] NOT NULL,
	[F1] [int] NOT NULL,
	[F2] [int] NOT NULL,
	[F3] [int] NOT NULL,
	[F4] [int] NOT NULL,
	[F5] [int] NOT NULL,
	[F6] [int] NOT NULL,
	[F7] [int] NOT NULL,
	[F8] [int] NOT NULL,
	[F9] [int] NOT NULL,
	[F10] [int] NOT NULL,
	[F11] [int] NOT NULL,
	[F12] [int] NOT NULL,
	[F13] [int] NOT NULL,
	[F14] [int] NOT NULL,
	[F15] [int] NOT NULL,
	[F16] [int] NOT NULL,
	[F17] [int] NOT NULL,
	[F18] [int] NOT NULL,
	[F19] [int] NOT NULL,
	[F20] [int] NOT NULL,
	[F21] [int] NOT NULL,
	[F22] [int] NOT NULL,
	[F23] [int] NOT NULL,
	[F24] [int] NOT NULL,
	[F25] [int] NOT NULL,
	[F26] [int] NOT NULL,
	[F27] [int] NOT NULL,
	[F28] [int] NOT NULL,
	[F29] [int] NOT NULL,
	[F30] [int] NOT NULL,
	[F31] [int] NOT NULL,
	[F32] [int] NOT NULL,
	[F33] [int] NOT NULL,
	[F34] [int] NOT NULL,
	[F35] [int] NOT NULL,
	[F36] [int] NOT NULL,
	[F37] [int] NOT NULL,
	[F38] [int] NOT NULL,
	[F39] [int] NOT NULL,
	[F40] [int] NOT NULL,
	[F41] [int] NOT NULL,
	[F42] [int] NOT NULL,
	[F43] [int] NOT NULL,
	[F44] [int] NOT NULL,
	[F45] [int] NOT NULL,
	[F46] [int] NOT NULL,
	[F47] [int] NOT NULL,
	[F48] [int] NOT NULL,
	[F49] [int] NOT NULL,
	[F50] [int] NOT NULL,
	[F51] [int] NOT NULL,
	[F52] [int] NOT NULL,
	[F53] [int] NOT NULL,
	[F54] [int] NOT NULL,
	[F55] [int] NOT NULL,
	[F56] [int] NOT NULL,
	[F57] [int] NOT NULL,
	[F58] [int] NOT NULL,
	[F59] [int] NOT NULL,
	[F60] [int] NOT NULL,
	[F61] [int] NOT NULL,
	[F62] [int] NOT NULL,
	[F63] [int] NOT NULL,
	[F64] [int] NOT NULL,
	[F65] [int] NOT NULL,
	[F66] [int] NOT NULL,
	[F67] [int] NOT NULL,
	[F68] [int] NOT NULL,
	[F69] [int] NOT NULL,
	[F70] [int] NOT NULL,
	[F71] [int] NOT NULL,
	[F72] [int] NOT NULL,
	[F73] [int] NOT NULL,
	[F74] [int] NOT NULL,
	[F75] [int] NOT NULL,
	[F76] [int] NOT NULL,
	[F77] [int] NOT NULL,
	[F78] [int] NOT NULL,
	[F79] [int] NOT NULL,
	[F80] [int] NOT NULL,
	[F81] [int] NOT NULL,
	[F82] [int] NOT NULL,
	[F83] [int] NOT NULL,
	[F84] [int] NOT NULL,
	[F85] [int] NOT NULL,
	[F86] [int] NOT NULL,
	[F87] [int] NOT NULL,
	[F88] [int] NOT NULL,
	[F89] [int] NOT NULL,
	[F90] [int] NOT NULL,
	[F91] [int] NOT NULL,
	[F92] [int] NOT NULL,
	[F93] [int] NOT NULL,
	[F94] [int] NOT NULL,
	[F95] [int] NOT NULL,
	[F96] [int] NOT NULL,
	[F97] [int] NOT NULL,
	[F98] [int] NOT NULL,
	[F99] [int] NOT NULL,
	[AK0] [tinyint] NOT NULL,
	[AD0] [bigint] NOT NULL,
	[AP0] [int] NOT NULL,
	[AK1] [tinyint] NOT NULL,
	[AD1] [bigint] NOT NULL,
	[AP1] [int] NOT NULL,
	[AK2] [tinyint] NOT NULL,
	[AD2] [bigint] NOT NULL,
	[AP2] [int] NOT NULL,
	[AK3] [tinyint] NOT NULL,
	[AD3] [bigint] NOT NULL,
	[AP3] [int] NOT NULL,
	[WM0] [int] NOT NULL,
	[WM1] [int] NOT NULL,
	[DM] [int] NOT NULL,
	[PM] [int] NOT NULL,
	[MI0] [int] NOT NULL,
	[MI1] [int] NOT NULL,
	[MI2] [int] NOT NULL,
	[SR] [int] NOT NULL,
	[FM0] [int] NOT NULL,
	[FM1] [int] NOT NULL,
	[FM2] [int] NOT NULL,
	[FM3] [int] NOT NULL,
	[FM4] [int] NOT NULL,
	[FM5] [int] NOT NULL,
	[FM6] [int] NOT NULL,
	[FM7] [int] NOT NULL,
	[FM8] [int] NOT NULL,
	[FM9] [int] NOT NULL,
	[FM10] [int] NOT NULL,
	[FM11] [int] NOT NULL,
	[FM12] [int] NOT NULL,
	[FM13] [int] NOT NULL,
	[FM14] [int] NOT NULL,
	[FM15] [int] NOT NULL,
	[FM16] [int] NOT NULL,
	[FM17] [int] NOT NULL,
	[FM18] [int] NOT NULL,
	[FM19] [int] NOT NULL,
	[FM20] [int] NOT NULL,
	[FM21] [int] NOT NULL,
	[FM22] [int] NOT NULL,
	[FM23] [int] NOT NULL,
	[SM0] [int] NOT NULL,
	[SM1] [int] NOT NULL,
	[SM2] [int] NOT NULL,
	[SM3] [int] NOT NULL,
	[SM4] [int] NOT NULL,
	[SM5] [int] NOT NULL,
	[SM6] [int] NOT NULL,
	[SM7] [int] NOT NULL,
	[SM8] [int] NOT NULL,
	[SM9] [int] NOT NULL,
	[SM10] [int] NOT NULL,
	[SM11] [int] NOT NULL,
	[SM12] [int] NOT NULL,
	[SM13] [int] NOT NULL,
	[SM14] [int] NOT NULL,
	[SM15] [int] NOT NULL,
	[SM16] [int] NOT NULL,
	[SM17] [int] NOT NULL,
	[SM18] [int] NOT NULL,
	[SM19] [int] NOT NULL,
	[SM20] [int] NOT NULL,
	[SM21] [int] NOT NULL,
	[SM22] [int] NOT NULL,
	[SM23] [int] NOT NULL,
	[SM24] [int] NOT NULL,
	[SM25] [int] NOT NULL,
	[SM26] [int] NOT NULL,
	[SM27] [int] NOT NULL,
	[SM28] [int] NOT NULL,
	[SM29] [int] NOT NULL,
	[SM30] [int] NOT NULL,
	[SM31] [int] NOT NULL,
	[SM32] [int] NOT NULL,
	[SM33] [int] NOT NULL,
	[SM34] [int] NOT NULL,
	[SM35] [int] NOT NULL,
	[SM36] [int] NOT NULL,
	[SM37] [int] NOT NULL,
	[SM38] [int] NOT NULL,
	[SM39] [int] NOT NULL,
	[SM40] [int] NOT NULL,
	[SM41] [int] NOT NULL,
	[SM42] [int] NOT NULL,
	[SM43] [int] NOT NULL,
	[SM44] [int] NOT NULL,
	[SM45] [int] NOT NULL,
	[SM46] [int] NOT NULL,
	[SM47] [int] NOT NULL,
	[SM48] [int] NOT NULL,
	[SM49] [int] NOT NULL,
	[LeftResList] [varchar](160) NOT NULL,
	[Class0] [smallint] NOT NULL,
	[Class1] [smallint] NOT NULL,
	[Class2] [smallint] NOT NULL,
	[ClassInitCnt] [int] NOT NULL,
	[LastClassGrade] [tinyint] NOT NULL,
	[PvpPoint] [float] NOT NULL,
	[PvpCashBag] [float] NOT NULL,
	[BindMapCode] [varchar](12) NOT NULL,
	[BindDummy] [varchar](12) NOT NULL,
	[GuildSerial] [int] NULL,
	[GuildGrade] [tinyint] NULL,
	[GuildRank] [smallint] NULL,
	[PvP_0] [float] NOT NULL,
	[PK_0] [int] NOT NULL,
	[PvP_1] [float] NOT NULL,
	[PK_1] [int] NOT NULL,
	[PvP_2] [float] NOT NULL,
	[PK_2] [int] NOT NULL,
	[CharacterGrade] [int] NOT NULL,
	[Die_0] [int] NOT NULL,
	[Die_1] [int] NOT NULL,
	[Die_2] [int] NOT NULL,
	[tmRaceBossVote] [datetime] NOT NULL,
	[TakeLastMentalTicket] [int] NOT NULL,
	[TakeLastCriTicket] [int] NOT NULL,
	[RDDelay] [int] NOT NULL,
	[ES0] [bigint] NOT NULL,
	[ET0] [int] NOT NULL,
	[ES1] [bigint] NOT NULL,
	[ET1] [int] NOT NULL,
	[ES2] [bigint] NOT NULL,
	[ET2] [int] NOT NULL,
	[ES3] [bigint] NOT NULL,
	[ET3] [int] NOT NULL,
	[ES4] [bigint] NOT NULL,
	[ET4] [int] NOT NULL,
	[ES5] [bigint] NOT NULL,
	[ET5] [int] NOT NULL,
	[AS0] [bigint] NOT NULL,
	[AS1] [bigint] NOT NULL,
	[AS2] [bigint] NOT NULL,
	[AS3] [bigint] NOT NULL,
	[FS0] [bigint] NOT NULL,
	[FS1] [bigint] NOT NULL,
	[FS2] [bigint] NOT NULL,
	[FS3] [bigint] NOT NULL,
	[FS4] [bigint] NOT NULL,
	[FS5] [bigint] NOT NULL,
	[FS6] [bigint] NOT NULL,
	[FS7] [bigint] NOT NULL,
	[FS8] [bigint] NOT NULL,
	[FS9] [bigint] NOT NULL,
	[FS10] [bigint] NOT NULL,
	[FS11] [bigint] NOT NULL,
	[FS12] [bigint] NOT NULL,
	[FS13] [bigint] NOT NULL,
	[FS14] [bigint] NOT NULL,
	[FS15] [bigint] NOT NULL,
	[FS16] [bigint] NOT NULL,
	[FS17] [bigint] NOT NULL,
	[FS18] [bigint] NOT NULL,
	[FS19] [bigint] NOT NULL,
	[FS20] [bigint] NOT NULL,
	[FS21] [bigint] NOT NULL,
	[FS22] [bigint] NOT NULL,
	[FS23] [bigint] NOT NULL,
	[FS24] [bigint] NOT NULL,
	[FS25] [bigint] NOT NULL,
	[FS26] [bigint] NOT NULL,
	[FS27] [bigint] NOT NULL,
	[FS28] [bigint] NOT NULL,
	[FS29] [bigint] NOT NULL,
	[FS30] [bigint] NOT NULL,
	[FS31] [bigint] NOT NULL,
	[FS32] [bigint] NOT NULL,
	[FS33] [bigint] NOT NULL,
	[FS34] [bigint] NOT NULL,
	[FS35] [bigint] NOT NULL,
	[FS36] [bigint] NOT NULL,
	[FS37] [bigint] NOT NULL,
	[FS38] [bigint] NOT NULL,
	[FS39] [bigint] NOT NULL,
	[FS40] [bigint] NOT NULL,
	[FS41] [bigint] NOT NULL,
	[FS42] [bigint] NOT NULL,
	[FS43] [bigint] NOT NULL,
	[FS44] [bigint] NOT NULL,
	[FS45] [bigint] NOT NULL,
	[FS46] [bigint] NOT NULL,
	[FS47] [bigint] NOT NULL,
	[FS48] [bigint] NOT NULL,
	[FS49] [bigint] NOT NULL,
	[FS50] [bigint] NOT NULL,
	[FS51] [bigint] NOT NULL,
	[FS52] [bigint] NOT NULL,
	[FS53] [bigint] NOT NULL,
	[FS54] [bigint] NOT NULL,
	[FS55] [bigint] NOT NULL,
	[FS56] [bigint] NOT NULL,
	[FS57] [bigint] NOT NULL,
	[FS58] [bigint] NOT NULL,
	[FS59] [bigint] NOT NULL,
	[FS60] [bigint] NOT NULL,
	[FS61] [bigint] NOT NULL,
	[FS62] [bigint] NOT NULL,
	[FS63] [bigint] NOT NULL,
	[FS64] [bigint] NOT NULL,
	[FS65] [bigint] NOT NULL,
	[FS66] [bigint] NOT NULL,
	[FS67] [bigint] NOT NULL,
	[FS68] [bigint] NOT NULL,
	[FS69] [bigint] NOT NULL,
	[FS70] [bigint] NOT NULL,
	[FS71] [bigint] NOT NULL,
	[FS72] [bigint] NOT NULL,
	[FS73] [bigint] NOT NULL,
	[FS74] [bigint] NOT NULL,
	[FS75] [bigint] NOT NULL,
	[FS76] [bigint] NOT NULL,
	[FS77] [bigint] NOT NULL,
	[FS78] [bigint] NOT NULL,
	[FS79] [bigint] NOT NULL,
	[FS80] [bigint] NOT NULL,
	[FS81] [bigint] NOT NULL,
	[FS82] [bigint] NOT NULL,
	[FS83] [bigint] NOT NULL,
	[FS84] [bigint] NOT NULL,
	[FS85] [bigint] NOT NULL,
	[FS86] [bigint] NOT NULL,
	[FS87] [bigint] NOT NULL,
	[FS88] [bigint] NOT NULL,
	[FS89] [bigint] NOT NULL,
	[FS90] [bigint] NOT NULL,
	[FS91] [bigint] NOT NULL,
	[FS92] [bigint] NOT NULL,
	[FS93] [bigint] NOT NULL,
	[FS94] [bigint] NOT NULL,
	[FS95] [bigint] NOT NULL,
	[FS96] [bigint] NOT NULL,
	[FS97] [bigint] NOT NULL,
	[FS98] [bigint] NOT NULL,
	[FS99] [bigint] NOT NULL,
	[MaxLevel] [int] NOT NULL,
	[GuildStatus] [tinyint] NOT NULL,
 CONSTRAINT [PK_tbl_general] PRIMARY KEY CLUSTERED 
(
	[Serial] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[tbl_GoldBoxItem]    Script Date: 12/01/2026 16:57:19 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[tbl_GoldBoxItem](
	[serial] [int] IDENTITY(1,1) NOT NULL,
	[DCK] [tinyint] NOT NULL,
	[StarterBoxMax] [int] NOT NULL,
	[BoxItemK_1] [int] NOT NULL,
	[BoxItemMax_1] [int] NOT NULL,
	[LimItemNum_1] [tinyint] NOT NULL,
	[BoxItemK_2] [int] NOT NULL,
	[BoxItemMax_2] [int] NOT NULL,
	[LimItemNum_2] [tinyint] NOT NULL,
	[K1_0] [int] NOT NULL,
	[N1_0] [int] NOT NULL,
	[K1_1] [int] NOT NULL,
	[N1_1] [int] NOT NULL,
	[K1_2] [int] NOT NULL,
	[N1_2] [int] NOT NULL,
	[K1_3] [int] NOT NULL,
	[N1_3] [int] NOT NULL,
	[K1_4] [int] NOT NULL,
	[N1_4] [int] NOT NULL,
	[K1_5] [int] NOT NULL,
	[N1_5] [int] NOT NULL,
	[K1_6] [int] NOT NULL,
	[N1_6] [int] NOT NULL,
	[K1_7] [int] NOT NULL,
	[N1_7] [int] NOT NULL,
	[K1_8] [int] NOT NULL,
	[N1_8] [int] NOT NULL,
	[K1_9] [int] NOT NULL,
	[N1_9] [int] NOT NULL,
	[K1_10] [int] NOT NULL,
	[N1_10] [int] NOT NULL,
	[K1_11] [int] NOT NULL,
	[N1_11] [int] NOT NULL,
	[K1_12] [int] NOT NULL,
	[N1_12] [int] NOT NULL,
	[K1_13] [int] NOT NULL,
	[N1_13] [int] NOT NULL,
	[K1_14] [int] NOT NULL,
	[N1_14] [int] NOT NULL,
	[K1_15] [int] NOT NULL,
	[N1_15] [int] NOT NULL,
	[K1_16] [int] NOT NULL,
	[N1_16] [int] NOT NULL,
	[K1_17] [int] NOT NULL,
	[N1_17] [int] NOT NULL,
	[K1_18] [int] NOT NULL,
	[N1_18] [int] NOT NULL,
	[K1_19] [int] NOT NULL,
	[N1_19] [int] NOT NULL,
	[K1_20] [int] NOT NULL,
	[N1_20] [int] NOT NULL,
	[K1_21] [int] NOT NULL,
	[N1_21] [int] NOT NULL,
	[K1_22] [int] NOT NULL,
	[N1_22] [int] NOT NULL,
	[K1_23] [int] NOT NULL,
	[N1_23] [int] NOT NULL,
	[K1_24] [int] NOT NULL,
	[N1_24] [int] NOT NULL,
	[K1_25] [int] NOT NULL,
	[N1_25] [int] NOT NULL,
	[K1_26] [int] NOT NULL,
	[N1_26] [int] NOT NULL,
	[K1_27] [int] NOT NULL,
	[N1_27] [int] NOT NULL,
	[K1_28] [int] NOT NULL,
	[N1_28] [int] NOT NULL,
	[K1_29] [int] NOT NULL,
	[N1_29] [int] NOT NULL,
	[K1_30] [int] NOT NULL,
	[N1_30] [int] NOT NULL,
	[K1_31] [int] NOT NULL,
	[N1_31] [int] NOT NULL,
	[K1_32] [int] NOT NULL,
	[N1_32] [int] NOT NULL,
	[K1_33] [int] NOT NULL,
	[N1_33] [int] NOT NULL,
	[K1_34] [int] NOT NULL,
	[N1_34] [int] NOT NULL,
	[K1_35] [int] NOT NULL,
	[N1_35] [int] NOT NULL,
	[K1_36] [int] NOT NULL,
	[N1_36] [int] NOT NULL,
	[K1_37] [int] NOT NULL,
	[N1_37] [int] NOT NULL,
	[K1_38] [int] NOT NULL,
	[N1_38] [int] NOT NULL,
	[K1_39] [int] NOT NULL,
	[N1_39] [int] NOT NULL,
	[K1_40] [int] NOT NULL,
	[N1_40] [int] NOT NULL,
	[K1_41] [int] NOT NULL,
	[N1_41] [int] NOT NULL,
	[K1_42] [int] NOT NULL,
	[N1_42] [int] NOT NULL,
	[K1_43] [int] NOT NULL,
	[N1_43] [int] NOT NULL,
	[K1_44] [int] NOT NULL,
	[N1_44] [int] NOT NULL,
	[K1_45] [int] NOT NULL,
	[N1_45] [int] NOT NULL,
	[K1_46] [int] NOT NULL,
	[N1_46] [int] NOT NULL,
	[K1_47] [int] NOT NULL,
	[N1_47] [int] NOT NULL,
	[K1_48] [int] NOT NULL,
	[N1_48] [int] NOT NULL,
	[K1_49] [int] NOT NULL,
	[N1_49] [int] NOT NULL,
	[K1_50] [int] NOT NULL,
	[N1_50] [int] NOT NULL,
	[K1_51] [int] NOT NULL,
	[N1_51] [int] NOT NULL,
	[K1_52] [int] NOT NULL,
	[N1_52] [int] NOT NULL,
	[K1_53] [int] NOT NULL,
	[N1_53] [int] NOT NULL,
	[K1_54] [int] NOT NULL,
	[N1_54] [int] NOT NULL,
	[K1_55] [int] NOT NULL,
	[N1_55] [int] NOT NULL,
	[K1_56] [int] NOT NULL,
	[N1_56] [int] NOT NULL,
	[K1_57] [int] NOT NULL,
	[N1_57] [int] NOT NULL,
	[K1_58] [int] NOT NULL,
	[N1_58] [int] NOT NULL,
	[K1_59] [int] NOT NULL,
	[N1_59] [int] NOT NULL,
	[K1_60] [int] NOT NULL,
	[N1_60] [int] NOT NULL,
	[K1_61] [int] NOT NULL,
	[N1_61] [int] NOT NULL,
	[K1_62] [int] NOT NULL,
	[N1_62] [int] NOT NULL,
	[K1_63] [int] NOT NULL,
	[N1_63] [int] NOT NULL,
	[K1_64] [int] NOT NULL,
	[N1_64] [int] NOT NULL,
	[K1_65] [int] NOT NULL,
	[N1_65] [int] NOT NULL,
	[K1_66] [int] NOT NULL,
	[N1_66] [int] NOT NULL,
	[K1_67] [int] NOT NULL,
	[N1_67] [int] NOT NULL,
	[K1_68] [int] NOT NULL,
	[N1_68] [int] NOT NULL,
	[K1_69] [int] NOT NULL,
	[N1_69] [int] NOT NULL,
	[K1_70] [int] NOT NULL,
	[N1_70] [int] NOT NULL,
	[K1_71] [int] NOT NULL,
	[N1_71] [int] NOT NULL,
	[K1_72] [int] NOT NULL,
	[N1_72] [int] NOT NULL,
	[K1_73] [int] NOT NULL,
	[N1_73] [int] NOT NULL,
	[K1_74] [int] NOT NULL,
	[N1_74] [int] NOT NULL,
	[K1_75] [int] NOT NULL,
	[N1_75] [int] NOT NULL,
	[K1_76] [int] NOT NULL,
	[N1_76] [int] NOT NULL,
	[K1_77] [int] NOT NULL,
	[N1_77] [int] NOT NULL,
	[K1_78] [int] NOT NULL,
	[N1_78] [int] NOT NULL,
	[K1_79] [int] NOT NULL,
	[N1_79] [int] NOT NULL,
	[K1_80] [int] NOT NULL,
	[N1_80] [int] NOT NULL,
	[K1_81] [int] NOT NULL,
	[N1_81] [int] NOT NULL,
	[K1_82] [int] NOT NULL,
	[N1_82] [int] NOT NULL,
	[K1_83] [int] NOT NULL,
	[N1_83] [int] NOT NULL,
	[K1_84] [int] NOT NULL,
	[N1_84] [int] NOT NULL,
	[K1_85] [int] NOT NULL,
	[N1_85] [int] NOT NULL,
	[K1_86] [int] NOT NULL,
	[N1_86] [int] NOT NULL,
	[K1_87] [int] NOT NULL,
	[N1_87] [int] NOT NULL,
	[K1_88] [int] NOT NULL,
	[N1_88] [int] NOT NULL,
	[K1_89] [int] NOT NULL,
	[N1_89] [int] NOT NULL,
	[K1_90] [int] NOT NULL,
	[N1_90] [int] NOT NULL,
	[K1_91] [int] NOT NULL,
	[N1_91] [int] NOT NULL,
	[K1_92] [int] NOT NULL,
	[N1_92] [int] NOT NULL,
	[K1_93] [int] NOT NULL,
	[N1_93] [int] NOT NULL,
	[K1_94] [int] NOT NULL,
	[N1_94] [int] NOT NULL,
	[K1_95] [int] NOT NULL,
	[N1_95] [int] NOT NULL,
	[K1_96] [int] NOT NULL,
	[N1_96] [int] NOT NULL,
	[K1_97] [int] NOT NULL,
	[N1_97] [int] NOT NULL,
	[K1_98] [int] NOT NULL,
	[N1_98] [int] NOT NULL,
	[K1_99] [int] NOT NULL,
	[N1_99] [int] NOT NULL,
	[K2_0] [int] NOT NULL,
	[N2_0] [int] NOT NULL,
	[K2_1] [int] NOT NULL,
	[N2_1] [int] NOT NULL,
	[K2_2] [int] NOT NULL,
	[N2_2] [int] NOT NULL,
	[K2_3] [int] NOT NULL,
	[N2_3] [int] NOT NULL,
	[K2_4] [int] NOT NULL,
	[N2_4] [int] NOT NULL,
	[K2_5] [int] NOT NULL,
	[N2_5] [int] NOT NULL,
	[K2_6] [int] NOT NULL,
	[N2_6] [int] NOT NULL,
	[K2_7] [int] NOT NULL,
	[N2_7] [int] NOT NULL,
	[K2_8] [int] NOT NULL,
	[N2_8] [int] NOT NULL,
	[K2_9] [int] NOT NULL,
	[N2_9] [int] NOT NULL,
	[K2_10] [int] NOT NULL,
	[N2_10] [int] NOT NULL,
	[K2_11] [int] NOT NULL,
	[N2_11] [int] NOT NULL,
	[K2_12] [int] NOT NULL,
	[N2_12] [int] NOT NULL,
	[K2_13] [int] NOT NULL,
	[N2_13] [int] NOT NULL,
	[K2_14] [int] NOT NULL,
	[N2_14] [int] NOT NULL,
	[K2_15] [int] NOT NULL,
	[N2_15] [int] NOT NULL,
	[K2_16] [int] NOT NULL,
	[N2_16] [int] NOT NULL,
	[K2_17] [int] NOT NULL,
	[N2_17] [int] NOT NULL,
	[K2_18] [int] NOT NULL,
	[N2_18] [int] NOT NULL,
	[K2_19] [int] NOT NULL,
	[N2_19] [int] NOT NULL,
	[K2_20] [int] NOT NULL,
	[N2_20] [int] NOT NULL,
	[K2_21] [int] NOT NULL,
	[N2_21] [int] NOT NULL,
	[K2_22] [int] NOT NULL,
	[N2_22] [int] NOT NULL,
	[K2_23] [int] NOT NULL,
	[N2_23] [int] NOT NULL,
	[K2_24] [int] NOT NULL,
	[N2_24] [int] NOT NULL,
	[K2_25] [int] NOT NULL,
	[N2_25] [int] NOT NULL,
	[K2_26] [int] NOT NULL,
	[N2_26] [int] NOT NULL,
	[K2_27] [int] NOT NULL,
	[N2_27] [int] NOT NULL,
	[K2_28] [int] NOT NULL,
	[N2_28] [int] NOT NULL,
	[K2_29] [int] NOT NULL,
	[N2_29] [int] NOT NULL,
	[K2_30] [int] NOT NULL,
	[N2_30] [int] NOT NULL,
	[K2_31] [int] NOT NULL,
	[N2_31] [int] NOT NULL,
	[K2_32] [int] NOT NULL,
	[N2_32] [int] NOT NULL,
	[K2_33] [int] NOT NULL,
	[N2_33] [int] NOT NULL,
	[K2_34] [int] NOT NULL,
	[N2_34] [int] NOT NULL,
	[K2_35] [int] NOT NULL,
	[N2_35] [int] NOT NULL,
	[K2_36] [int] NOT NULL,
	[N2_36] [int] NOT NULL,
	[K2_37] [int] NOT NULL,
	[N2_37] [int] NOT NULL,
	[K2_38] [int] NOT NULL,
	[N2_38] [int] NOT NULL,
	[K2_39] [int] NOT NULL,
	[N2_39] [int] NOT NULL,
	[K2_40] [int] NOT NULL,
	[N2_40] [int] NOT NULL,
	[K2_41] [int] NOT NULL,
	[N2_41] [int] NOT NULL,
	[K2_42] [int] NOT NULL,
	[N2_42] [int] NOT NULL,
	[K2_43] [int] NOT NULL,
	[N2_43] [int] NOT NULL,
	[K2_44] [int] NOT NULL,
	[N2_44] [int] NOT NULL,
	[K2_45] [int] NOT NULL,
	[N2_45] [int] NOT NULL,
	[K2_46] [int] NOT NULL,
	[N2_46] [int] NOT NULL,
	[K2_47] [int] NOT NULL,
	[N2_47] [int] NOT NULL,
	[K2_48] [int] NOT NULL,
	[N2_48] [int] NOT NULL,
	[K2_49] [int] NOT NULL,
	[N2_49] [int] NOT NULL,
	[K2_50] [int] NOT NULL,
	[N2_50] [int] NOT NULL,
	[K2_51] [int] NOT NULL,
	[N2_51] [int] NOT NULL,
	[K2_52] [int] NOT NULL,
	[N2_52] [int] NOT NULL,
	[K2_53] [int] NOT NULL,
	[N2_53] [int] NOT NULL,
	[K2_54] [int] NOT NULL,
	[N2_54] [int] NOT NULL,
	[K2_55] [int] NOT NULL,
	[N2_55] [int] NOT NULL,
	[K2_56] [int] NOT NULL,
	[N2_56] [int] NOT NULL,
	[K2_57] [int] NOT NULL,
	[N2_57] [int] NOT NULL,
	[K2_58] [int] NOT NULL,
	[N2_58] [int] NOT NULL,
	[K2_59] [int] NOT NULL,
	[N2_59] [int] NOT NULL,
	[K2_60] [int] NOT NULL,
	[N2_60] [int] NOT NULL,
	[K2_61] [int] NOT NULL,
	[N2_61] [int] NOT NULL,
	[K2_62] [int] NOT NULL,
	[N2_62] [int] NOT NULL,
	[K2_63] [int] NOT NULL,
	[N2_63] [int] NOT NULL,
	[K2_64] [int] NOT NULL,
	[N2_64] [int] NOT NULL,
	[K2_65] [int] NOT NULL,
	[N2_65] [int] NOT NULL,
	[K2_66] [int] NOT NULL,
	[N2_66] [int] NOT NULL,
	[K2_67] [int] NOT NULL,
	[N2_67] [int] NOT NULL,
	[K2_68] [int] NOT NULL,
	[N2_68] [int] NOT NULL,
	[K2_69] [int] NOT NULL,
	[N2_69] [int] NOT NULL,
	[K2_70] [int] NOT NULL,
	[N2_70] [int] NOT NULL,
	[K2_71] [int] NOT NULL,
	[N2_71] [int] NOT NULL,
	[K2_72] [int] NOT NULL,
	[N2_72] [int] NOT NULL,
	[K2_73] [int] NOT NULL,
	[N2_73] [int] NOT NULL,
	[K2_74] [int] NOT NULL,
	[N2_74] [int] NOT NULL,
	[K2_75] [int] NOT NULL,
	[N2_75] [int] NOT NULL,
	[K2_76] [int] NOT NULL,
	[N2_76] [int] NOT NULL,
	[K2_77] [int] NOT NULL,
	[N2_77] [int] NOT NULL,
	[K2_78] [int] NOT NULL,
	[N2_78] [int] NOT NULL,
	[K2_79] [int] NOT NULL,
	[N2_79] [int] NOT NULL,
	[K2_80] [int] NOT NULL,
	[N2_80] [int] NOT NULL,
	[K2_81] [int] NOT NULL,
	[N2_81] [int] NOT NULL,
	[K2_82] [int] NOT NULL,
	[N2_82] [int] NOT NULL,
	[K2_83] [int] NOT NULL,
	[N2_83] [int] NOT NULL,
	[K2_84] [int] NOT NULL,
	[N2_84] [int] NOT NULL,
	[K2_85] [int] NOT NULL,
	[N2_85] [int] NOT NULL,
	[K2_86] [int] NOT NULL,
	[N2_86] [int] NOT NULL,
	[K2_87] [int] NOT NULL,
	[N2_87] [int] NOT NULL,
	[K2_88] [int] NOT NULL,
	[N2_88] [int] NOT NULL,
	[K2_89] [int] NOT NULL,
	[N2_89] [int] NOT NULL,
	[K2_90] [int] NOT NULL,
	[N2_90] [int] NOT NULL,
	[K2_91] [int] NOT NULL,
	[N2_91] [int] NOT NULL,
	[K2_92] [int] NOT NULL,
	[N2_92] [int] NOT NULL,
	[K2_93] [int] NOT NULL,
	[N2_93] [int] NOT NULL,
	[K2_94] [int] NOT NULL,
	[N2_94] [int] NOT NULL,
	[K2_95] [int] NOT NULL,
	[N2_95] [int] NOT NULL,
	[K2_96] [int] NOT NULL,
	[N2_96] [int] NOT NULL,
	[K2_97] [int] NOT NULL,
	[N2_97] [int] NOT NULL,
	[K2_98] [int] NOT NULL,
	[N2_98] [int] NOT NULL,
	[K2_99] [int] NOT NULL,
	[N2_99] [int] NOT NULL,
 CONSTRAINT [PK_tbl_GoldBoxItem] PRIMARY KEY CLUSTERED 
(
	[serial] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[tbl_GreetMsg]    Script Date: 12/01/2026 16:57:19 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[tbl_GreetMsg](
	[useType] [tinyint] NOT NULL,
	[GMsg] [varchar](255) NOT NULL,
	[Name] [varchar](17) NOT NULL,
 CONSTRAINT [Pk_tbl_GreetMsg] PRIMARY KEY CLUSTERED 
(
	[useType] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[tbl_Guild]    Script Date: 12/01/2026 16:57:19 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[tbl_Guild](
	[Serial] [int] IDENTITY(1,1) NOT NULL,
	[id] [varchar](17) NOT NULL,
	[Profile] [varchar](128) NOT NULL,
	[CreateDt] [datetime] NOT NULL,
	[Grade] [smallint] NOT NULL,
	[DCK] [tinyint] NOT NULL,
	[Dalant] [float] NULL,
	[Gold] [float] NULL,
	[EmblemBack] [int] NOT NULL,
	[EmblemMark] [int] NOT NULL,
	[Race] [tinyint] NULL,
	[MemberCount] [smallint] NULL,
	[MasterSerial] [int] NOT NULL,
	[MasterBeforeGrade] [tinyint] NOT NULL,
	[GMsg] [varchar](255) NOT NULL,
	[deleteid] [varchar](17) NOT NULL,
 CONSTRAINT [PK_tbl_Guild] PRIMARY KEY CLUSTERED 
(
	[Serial] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY],
 CONSTRAINT [IX_tbl_Guild] UNIQUE NONCLUSTERED 
(
	[id] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[tbl_GuildBattleRank]    Script Date: 12/01/2026 16:57:19 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[tbl_GuildBattleRank](
	[serial] [int] NOT NULL,
	[dck] [bit] NOT NULL,
	[win] [int] NOT NULL,
	[lose] [int] NOT NULL,
	[draw] [int] NOT NULL,
	[score] [int] NOT NULL,
	[totwin] [int] NOT NULL,
	[totlose] [int] NOT NULL,
	[totdraw] [int] NOT NULL,
	[totcnt] [int] NOT NULL,
 CONSTRAINT [PK_tbl_GuildRank] PRIMARY KEY CLUSTERED 
(
	[serial] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[tbl_guildbattleresullog]    Script Date: 12/01/2026 16:57:19 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[tbl_guildbattleresullog](
	[serial] [int] IDENTITY(1,1) NOT NULL,
	[start] [smalldatetime] NOT NULL,
	[end] [smalldatetime] NOT NULL,
	[redserial] [int] NOT NULL,
	[redname] [varchar](17) NOT NULL,
	[blueserial] [int] NOT NULL,
	[bluename] [varchar](17) NOT NULL,
	[redscore] [int] NOT NULL,
	[bluescore] [int] NOT NULL,
	[redmaxjoinmembercnt] [int] NOT NULL,
	[bluemaxjoinmembercnt] [int] NOT NULL,
	[redgoalcnt] [int] NOT NULL,
	[bluegoalcnt] [int] NOT NULL,
	[redkillcnt] [int] NOT NULL,
	[bluekillcnt] [int] NOT NULL,
	[result] [tinyint] NOT NULL,
	[topgoalcharacserial] [int] NOT NULL,
	[topgoalcharacname] [varchar](17) NOT NULL,
	[topkillcharacserial] [int] NOT NULL,
	[topkillcharacname] [varchar](17) NOT NULL,
	[joinlimit] [tinyint] NOT NULL,
	[guildbattlecostgold] [int] NOT NULL,
	[mapcode] [varchar](12) NOT NULL,
	[redweeklywin] [int] NULL,
	[redweeklylose] [int] NULL,
	[redweeklydraw] [int] NULL,
	[redweeklywinscore] [int] NULL,
	[redtotwin] [int] NULL,
	[redtotlose] [int] NULL,
	[redtotdraw] [int] NULL,
	[redtotbattlecnt] [int] NULL,
	[blueweeklywin] [int] NULL,
	[blueweeklylose] [int] NULL,
	[blueweeklydraw] [int] NULL,
	[blueweeklywinscore] [int] NULL,
	[bluetotwin] [int] NULL,
	[bluetotlose] [int] NULL,
	[bluetotdraw] [int] NULL,
	[bluetotbattlecnt] [int] NULL,
	[redguildbattlepvppoint] [float] NULL,
	[blueguildbattlepvppoint] [float] NULL,
 CONSTRAINT [PK_tbl_guildbattleresullog_start_serial] PRIMARY KEY CLUSTERED 
(
	[start] ASC,
	[serial] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[tbl_GuildBattleScheduleInfo]    Script Date: 12/01/2026 16:57:19 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[tbl_GuildBattleScheduleInfo](
	[ID] [int] NOT NULL,
	[SLID] [int] NOT NULL,
	[State] [tinyint] NOT NULL,
	[StartTime] [datetime] NOT NULL,
	[BattleTurm] [smallint] NOT NULL,
 CONSTRAINT [pk_tbl_guildbattlescheduleinfo] PRIMARY KEY CLUSTERED 
(
	[ID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[tbl_GuildMoneyHistory_Log]    Script Date: 12/01/2026 16:57:19 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[tbl_GuildMoneyHistory_Log](
	[Serial] [int] IDENTITY(1,1) NOT NULL,
	[GuildSerial] [int] NOT NULL,
	[InoutDalant] [float] NOT NULL,
	[InoutGold] [float] NOT NULL,
	[ResultDalant] [float] NOT NULL,
	[ResultGold] [float] NOT NULL,
	[LogDate] [char](8) NOT NULL,
	[AvatorSerial] [int] NOT NULL,
	[AvatorName] [varchar](17) NULL,
 CONSTRAINT [PK_tbl_GuildMoneyHistory_Log] PRIMARY KEY CLUSTERED 
(
	[Serial] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[tbl_GuildRankToday]    Script Date: 12/01/2026 16:57:19 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[tbl_GuildRankToday](
	[Rank] [int] IDENTITY(1,1) NOT NULL,
	[Rate] [int] NOT NULL,
	[Serial] [int] NULL,
	[GuildPower] [float] NULL,
	[Grade] [int] NOT NULL
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[tbl_GuildRoom]    Script Date: 12/01/2026 16:57:19 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[tbl_GuildRoom](
	[Serial] [int] IDENTITY(1,1) NOT NULL,
	[Dck] [bit] NOT NULL,
	[GuildSerial] [int] NOT NULL,
	[RoomType] [tinyint] NOT NULL,
	[RoomRace] [tinyint] NOT NULL,
	[RoomRentDate] [datetime] NOT NULL,
	[LogDate] [datetime] NOT NULL,
 CONSTRAINT [PK_tbl_GuildRoom] PRIMARY KEY CLUSTERED 
(
	[Serial] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[tbl_history_electraceboss_0]    Script Date: 12/01/2026 16:57:19 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[tbl_history_electraceboss_0](
	[serial] [int] IDENTITY(1,1) NOT NULL,
	[WorldName] [varchar](32) NOT NULL,
	[tmCreate] [datetime] NOT NULL,
	[bOnElect] [bit] NOT NULL,
	[bOnCandidate] [bit] NOT NULL,
	[bOnVote] [bit] NOT NULL,
	[tmOnElect] [datetime] NOT NULL,
	[tmOnCandidate] [datetime] NOT NULL,
	[tmOnVote] [datetime] NOT NULL,
	[TotalVoteCnt] [int] NOT NULL,
	[tmLastUpdate] [datetime] NOT NULL,
 CONSTRAINT [PK_tbl_history_electraceboss_0] PRIMARY KEY CLUSTERED 
(
	[serial] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[tbl_history_electraceboss_1]    Script Date: 12/01/2026 16:57:19 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[tbl_history_electraceboss_1](
	[serial] [int] IDENTITY(1,1) NOT NULL,
	[WorldName] [varchar](32) NOT NULL,
	[tmCreate] [datetime] NOT NULL,
	[bOnElect] [bit] NOT NULL,
	[bOnCandidate] [bit] NOT NULL,
	[bOnVote] [bit] NOT NULL,
	[tmOnElect] [datetime] NOT NULL,
	[tmOnCandidate] [datetime] NOT NULL,
	[tmOnVote] [datetime] NOT NULL,
	[TotalVoteCnt] [int] NOT NULL,
	[tmLastUpdate] [datetime] NOT NULL,
 CONSTRAINT [PK_tbl_history_electraceboss_1] PRIMARY KEY CLUSTERED 
(
	[serial] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[tbl_history_electraceboss_2]    Script Date: 12/01/2026 16:57:19 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[tbl_history_electraceboss_2](
	[serial] [int] IDENTITY(1,1) NOT NULL,
	[WorldName] [varchar](32) NOT NULL,
	[tmCreate] [datetime] NOT NULL,
	[bOnElect] [bit] NOT NULL,
	[bOnCandidate] [bit] NOT NULL,
	[bOnVote] [bit] NOT NULL,
	[tmOnElect] [datetime] NOT NULL,
	[tmOnCandidate] [datetime] NOT NULL,
	[tmOnVote] [datetime] NOT NULL,
	[TotalVoteCnt] [int] NOT NULL,
	[tmLastUpdate] [datetime] NOT NULL,
 CONSTRAINT [PK_tbl_history_electraceboss_2] PRIMARY KEY CLUSTERED 
(
	[serial] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[tbl_history_electraceboss_candidacy_0]    Script Date: 12/01/2026 16:57:19 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[tbl_history_electraceboss_candidacy_0](
	[nID] [int] IDENTITY(1,1) NOT NULL,
	[serial] [int] NOT NULL,
	[bGiveback] [bit] NOT NULL,
	[bPrevBoss] [bit] NOT NULL,
	[bregSecond] [bit] NOT NULL,
	[tmReg_F] [datetime] NOT NULL,
	[tmReg_S] [datetime] NOT NULL,
	[AvatorSerial] [int] NOT NULL,
	[AvatorName] [varchar](17) NOT NULL,
	[GuildSerial] [int] NOT NULL,
	[GuildName] [varchar](17) NOT NULL,
	[Rank] [tinyint] NOT NULL,
	[Score] [int] NOT NULL,
	[WinCnt] [int] NOT NULL,
 CONSTRAINT [PK_tbl_history_electraceboss_candidacy_0] PRIMARY KEY CLUSTERED 
(
	[nID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[tbl_history_electraceboss_candidacy_1]    Script Date: 12/01/2026 16:57:19 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[tbl_history_electraceboss_candidacy_1](
	[nID] [int] IDENTITY(1,1) NOT NULL,
	[serial] [int] NOT NULL,
	[bGiveback] [bit] NOT NULL,
	[bPrevBoss] [bit] NOT NULL,
	[bregSecond] [bit] NOT NULL,
	[tmReg_F] [datetime] NOT NULL,
	[tmReg_S] [datetime] NOT NULL,
	[AvatorSerial] [int] NOT NULL,
	[AvatorName] [varchar](17) NOT NULL,
	[GuildSerial] [int] NOT NULL,
	[GuildName] [varchar](17) NOT NULL,
	[Rank] [tinyint] NOT NULL,
	[Score] [int] NOT NULL,
	[WinCnt] [int] NOT NULL,
 CONSTRAINT [PK_tbl_history_electraceboss_candidacy_1] PRIMARY KEY CLUSTERED 
(
	[nID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[tbl_history_electraceboss_candidacy_2]    Script Date: 12/01/2026 16:57:19 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[tbl_history_electraceboss_candidacy_2](
	[nID] [int] IDENTITY(1,1) NOT NULL,
	[serial] [int] NOT NULL,
	[bGiveback] [bit] NOT NULL,
	[bPrevBoss] [bit] NOT NULL,
	[bregSecond] [bit] NOT NULL,
	[tmReg_F] [datetime] NOT NULL,
	[tmReg_S] [datetime] NOT NULL,
	[AvatorSerial] [int] NOT NULL,
	[AvatorName] [varchar](17) NOT NULL,
	[GuildSerial] [int] NOT NULL,
	[GuildName] [varchar](17) NOT NULL,
	[Rank] [tinyint] NOT NULL,
	[Score] [int] NOT NULL,
	[WinCnt] [int] NOT NULL,
 CONSTRAINT [PK_tbl_history_electraceboss_candidacy_2] PRIMARY KEY CLUSTERED 
(
	[nID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[tbl_history_electraceboss_winner_0]    Script Date: 12/01/2026 16:57:19 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[tbl_history_electraceboss_winner_0](
	[nID] [int] IDENTITY(1,1) NOT NULL,
	[serial] [int] NOT NULL,
	[AvatorSerial] [int] NOT NULL,
	[AvatorName] [varchar](17) NOT NULL,
	[Rank] [tinyint] NOT NULL,
 CONSTRAINT [PK_tbl_history_electraceboss_winner_0] PRIMARY KEY CLUSTERED 
(
	[nID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[tbl_history_electraceboss_winner_1]    Script Date: 12/01/2026 16:57:19 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[tbl_history_electraceboss_winner_1](
	[nID] [int] IDENTITY(1,1) NOT NULL,
	[serial] [int] NOT NULL,
	[AvatorSerial] [int] NOT NULL,
	[AvatorName] [varchar](17) NOT NULL,
	[Rank] [tinyint] NOT NULL,
 CONSTRAINT [PK_tbl_history_electraceboss_winner_1] PRIMARY KEY CLUSTERED 
(
	[nID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[tbl_history_electraceboss_winner_2]    Script Date: 12/01/2026 16:57:19 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[tbl_history_electraceboss_winner_2](
	[nID] [int] IDENTITY(1,1) NOT NULL,
	[serial] [int] NOT NULL,
	[AvatorSerial] [int] NOT NULL,
	[AvatorName] [varchar](17) NOT NULL,
	[Rank] [tinyint] NOT NULL,
 CONSTRAINT [PK_tbl_history_electraceboss_winner_2] PRIMARY KEY CLUSTERED 
(
	[nID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[tbl_honor_guild]    Script Date: 12/01/2026 16:57:19 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[tbl_honor_guild](
	[idx] [int] IDENTITY(1,1) NOT NULL,
	[Race] [tinyint] NOT NULL,
	[GuildSerial] [int] NOT NULL,
	[TaxRate] [tinyint] NOT NULL,
	[IsNext] [tinyint] NOT NULL,
	[DCK] [tinyint] NOT NULL,
	[dtUpdate] [datetime] NOT NULL,
	[dtDisable] [datetime] NULL,
 CONSTRAINT [PK_tbl_honor_guild] PRIMARY KEY CLUSTERED 
(
	[idx] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[tbl_inven]    Script Date: 12/01/2026 16:57:19 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[tbl_inven](
	[Serial] [int] NOT NULL,
	[DCK] [tinyint] NOT NULL,
	[K0] [int] NOT NULL,
	[D0] [bigint] NOT NULL,
	[U0] [int] NOT NULL,
	[K1] [int] NOT NULL,
	[D1] [bigint] NOT NULL,
	[U1] [int] NOT NULL,
	[K2] [int] NOT NULL,
	[D2] [bigint] NOT NULL,
	[U2] [int] NOT NULL,
	[K3] [int] NOT NULL,
	[D3] [bigint] NOT NULL,
	[U3] [int] NOT NULL,
	[K4] [int] NOT NULL,
	[D4] [bigint] NOT NULL,
	[U4] [int] NOT NULL,
	[K5] [int] NOT NULL,
	[D5] [bigint] NOT NULL,
	[U5] [int] NOT NULL,
	[K6] [int] NOT NULL,
	[D6] [bigint] NOT NULL,
	[U6] [int] NOT NULL,
	[K7] [int] NOT NULL,
	[D7] [bigint] NOT NULL,
	[U7] [int] NOT NULL,
	[K8] [int] NOT NULL,
	[D8] [bigint] NOT NULL,
	[U8] [int] NOT NULL,
	[K9] [int] NOT NULL,
	[D9] [bigint] NOT NULL,
	[U9] [int] NOT NULL,
	[K10] [int] NOT NULL,
	[D10] [bigint] NOT NULL,
	[U10] [int] NOT NULL,
	[K11] [int] NOT NULL,
	[D11] [bigint] NOT NULL,
	[U11] [int] NOT NULL,
	[K12] [int] NOT NULL,
	[D12] [bigint] NOT NULL,
	[U12] [int] NOT NULL,
	[K13] [int] NOT NULL,
	[D13] [bigint] NOT NULL,
	[U13] [int] NOT NULL,
	[K14] [int] NOT NULL,
	[D14] [bigint] NOT NULL,
	[U14] [int] NOT NULL,
	[K15] [int] NOT NULL,
	[D15] [bigint] NOT NULL,
	[U15] [int] NOT NULL,
	[K16] [int] NOT NULL,
	[D16] [bigint] NOT NULL,
	[U16] [int] NOT NULL,
	[K17] [int] NOT NULL,
	[D17] [bigint] NOT NULL,
	[U17] [int] NOT NULL,
	[K18] [int] NOT NULL,
	[D18] [bigint] NOT NULL,
	[U18] [int] NOT NULL,
	[K19] [int] NOT NULL,
	[D19] [bigint] NOT NULL,
	[U19] [int] NOT NULL,
	[K20] [int] NOT NULL,
	[D20] [bigint] NOT NULL,
	[U20] [int] NOT NULL,
	[K21] [int] NOT NULL,
	[D21] [bigint] NOT NULL,
	[U21] [int] NOT NULL,
	[K22] [int] NOT NULL,
	[D22] [bigint] NOT NULL,
	[U22] [int] NOT NULL,
	[K23] [int] NOT NULL,
	[D23] [bigint] NOT NULL,
	[U23] [int] NOT NULL,
	[K24] [int] NOT NULL,
	[D24] [bigint] NOT NULL,
	[U24] [int] NOT NULL,
	[K25] [int] NOT NULL,
	[D25] [bigint] NOT NULL,
	[U25] [int] NOT NULL,
	[K26] [int] NOT NULL,
	[D26] [bigint] NOT NULL,
	[U26] [int] NOT NULL,
	[K27] [int] NOT NULL,
	[D27] [bigint] NOT NULL,
	[U27] [int] NOT NULL,
	[K28] [int] NOT NULL,
	[D28] [bigint] NOT NULL,
	[U28] [int] NOT NULL,
	[K29] [int] NOT NULL,
	[D29] [bigint] NOT NULL,
	[U29] [int] NOT NULL,
	[K30] [int] NOT NULL,
	[D30] [bigint] NOT NULL,
	[U30] [int] NOT NULL,
	[K31] [int] NOT NULL,
	[D31] [bigint] NOT NULL,
	[U31] [int] NOT NULL,
	[K32] [int] NOT NULL,
	[D32] [bigint] NOT NULL,
	[U32] [int] NOT NULL,
	[K33] [int] NOT NULL,
	[D33] [bigint] NOT NULL,
	[U33] [int] NOT NULL,
	[K34] [int] NOT NULL,
	[D34] [bigint] NOT NULL,
	[U34] [int] NOT NULL,
	[K35] [int] NOT NULL,
	[D35] [bigint] NOT NULL,
	[U35] [int] NOT NULL,
	[K36] [int] NOT NULL,
	[D36] [bigint] NOT NULL,
	[U36] [int] NOT NULL,
	[K37] [int] NOT NULL,
	[D37] [bigint] NOT NULL,
	[U37] [int] NOT NULL,
	[K38] [int] NOT NULL,
	[D38] [bigint] NOT NULL,
	[U38] [int] NOT NULL,
	[K39] [int] NOT NULL,
	[D39] [bigint] NOT NULL,
	[U39] [int] NOT NULL,
	[K40] [int] NOT NULL,
	[D40] [bigint] NOT NULL,
	[U40] [int] NOT NULL,
	[K41] [int] NOT NULL,
	[D41] [bigint] NOT NULL,
	[U41] [int] NOT NULL,
	[K42] [int] NOT NULL,
	[D42] [bigint] NOT NULL,
	[U42] [int] NOT NULL,
	[K43] [int] NOT NULL,
	[D43] [bigint] NOT NULL,
	[U43] [int] NOT NULL,
	[K44] [int] NOT NULL,
	[D44] [bigint] NOT NULL,
	[U44] [int] NOT NULL,
	[K45] [int] NOT NULL,
	[D45] [bigint] NOT NULL,
	[U45] [int] NOT NULL,
	[K46] [int] NOT NULL,
	[D46] [bigint] NOT NULL,
	[U46] [int] NOT NULL,
	[K47] [int] NOT NULL,
	[D47] [bigint] NOT NULL,
	[U47] [int] NOT NULL,
	[K48] [int] NOT NULL,
	[D48] [bigint] NOT NULL,
	[U48] [int] NOT NULL,
	[K49] [int] NOT NULL,
	[D49] [bigint] NOT NULL,
	[U49] [int] NOT NULL,
	[K50] [int] NOT NULL,
	[D50] [bigint] NOT NULL,
	[U50] [int] NOT NULL,
	[K51] [int] NOT NULL,
	[D51] [bigint] NOT NULL,
	[U51] [int] NOT NULL,
	[K52] [int] NOT NULL,
	[D52] [bigint] NOT NULL,
	[U52] [int] NOT NULL,
	[K53] [int] NOT NULL,
	[D53] [bigint] NOT NULL,
	[U53] [int] NOT NULL,
	[K54] [int] NOT NULL,
	[D54] [bigint] NOT NULL,
	[U54] [int] NOT NULL,
	[K55] [int] NOT NULL,
	[D55] [bigint] NOT NULL,
	[U55] [int] NOT NULL,
	[K56] [int] NOT NULL,
	[D56] [bigint] NOT NULL,
	[U56] [int] NOT NULL,
	[K57] [int] NOT NULL,
	[D57] [bigint] NOT NULL,
	[U57] [int] NOT NULL,
	[K58] [int] NOT NULL,
	[D58] [bigint] NOT NULL,
	[U58] [int] NOT NULL,
	[K59] [int] NOT NULL,
	[D59] [bigint] NOT NULL,
	[U59] [int] NOT NULL,
	[K60] [int] NOT NULL,
	[D60] [bigint] NOT NULL,
	[U60] [int] NOT NULL,
	[K61] [int] NOT NULL,
	[D61] [bigint] NOT NULL,
	[U61] [int] NOT NULL,
	[K62] [int] NOT NULL,
	[D62] [bigint] NOT NULL,
	[U62] [int] NOT NULL,
	[K63] [int] NOT NULL,
	[D63] [bigint] NOT NULL,
	[U63] [int] NOT NULL,
	[K64] [int] NOT NULL,
	[D64] [bigint] NOT NULL,
	[U64] [int] NOT NULL,
	[K65] [int] NOT NULL,
	[D65] [bigint] NOT NULL,
	[U65] [int] NOT NULL,
	[K66] [int] NOT NULL,
	[D66] [bigint] NOT NULL,
	[U66] [int] NOT NULL,
	[K67] [int] NOT NULL,
	[D67] [bigint] NOT NULL,
	[U67] [int] NOT NULL,
	[K68] [int] NOT NULL,
	[D68] [bigint] NOT NULL,
	[U68] [int] NOT NULL,
	[K69] [int] NOT NULL,
	[D69] [bigint] NOT NULL,
	[U69] [int] NOT NULL,
	[K70] [int] NOT NULL,
	[D70] [bigint] NOT NULL,
	[U70] [int] NOT NULL,
	[K71] [int] NOT NULL,
	[D71] [bigint] NOT NULL,
	[U71] [int] NOT NULL,
	[K72] [int] NOT NULL,
	[D72] [bigint] NOT NULL,
	[U72] [int] NOT NULL,
	[K73] [int] NOT NULL,
	[D73] [bigint] NOT NULL,
	[U73] [int] NOT NULL,
	[K74] [int] NOT NULL,
	[D74] [bigint] NOT NULL,
	[U74] [int] NOT NULL,
	[K75] [int] NOT NULL,
	[D75] [bigint] NOT NULL,
	[U75] [int] NOT NULL,
	[K76] [int] NOT NULL,
	[D76] [bigint] NOT NULL,
	[U76] [int] NOT NULL,
	[K77] [int] NOT NULL,
	[D77] [bigint] NOT NULL,
	[U77] [int] NOT NULL,
	[K78] [int] NOT NULL,
	[D78] [bigint] NOT NULL,
	[U78] [int] NOT NULL,
	[K79] [int] NOT NULL,
	[D79] [bigint] NOT NULL,
	[U79] [int] NOT NULL,
	[K80] [int] NOT NULL,
	[D80] [bigint] NOT NULL,
	[U80] [int] NOT NULL,
	[K81] [int] NOT NULL,
	[D81] [bigint] NOT NULL,
	[U81] [int] NOT NULL,
	[K82] [int] NOT NULL,
	[D82] [bigint] NOT NULL,
	[U82] [int] NOT NULL,
	[K83] [int] NOT NULL,
	[D83] [bigint] NOT NULL,
	[U83] [int] NOT NULL,
	[K84] [int] NOT NULL,
	[D84] [bigint] NOT NULL,
	[U84] [int] NOT NULL,
	[K85] [int] NOT NULL,
	[D85] [bigint] NOT NULL,
	[U85] [int] NOT NULL,
	[K86] [int] NOT NULL,
	[D86] [bigint] NOT NULL,
	[U86] [int] NOT NULL,
	[K87] [int] NOT NULL,
	[D87] [bigint] NOT NULL,
	[U87] [int] NOT NULL,
	[K88] [int] NOT NULL,
	[D88] [bigint] NOT NULL,
	[U88] [int] NOT NULL,
	[K89] [int] NOT NULL,
	[D89] [bigint] NOT NULL,
	[U89] [int] NOT NULL,
	[K90] [int] NOT NULL,
	[D90] [bigint] NOT NULL,
	[U90] [int] NOT NULL,
	[K91] [int] NOT NULL,
	[D91] [bigint] NOT NULL,
	[U91] [int] NOT NULL,
	[K92] [int] NOT NULL,
	[D92] [bigint] NOT NULL,
	[U92] [int] NOT NULL,
	[K93] [int] NOT NULL,
	[D93] [bigint] NOT NULL,
	[U93] [int] NOT NULL,
	[K94] [int] NOT NULL,
	[D94] [bigint] NOT NULL,
	[U94] [int] NOT NULL,
	[K95] [int] NOT NULL,
	[D95] [bigint] NOT NULL,
	[U95] [int] NOT NULL,
	[K96] [int] NOT NULL,
	[D96] [bigint] NOT NULL,
	[U96] [int] NOT NULL,
	[K97] [int] NOT NULL,
	[D97] [bigint] NOT NULL,
	[U97] [int] NOT NULL,
	[K98] [int] NOT NULL,
	[D98] [bigint] NOT NULL,
	[U98] [int] NOT NULL,
	[K99] [int] NOT NULL,
	[D99] [bigint] NOT NULL,
	[U99] [int] NOT NULL,
	[TrIndex0] [tinyint] NOT NULL,
	[TrPrice0] [int] NOT NULL,
	[TrPass0] [int] NOT NULL,
	[TrIndex1] [tinyint] NOT NULL,
	[TrPrice1] [int] NOT NULL,
	[TrPass1] [int] NOT NULL,
	[TrIndex2] [tinyint] NOT NULL,
	[TrPrice2] [int] NOT NULL,
	[TrPass2] [int] NOT NULL,
	[TrIndex3] [tinyint] NOT NULL,
	[TrPrice3] [int] NOT NULL,
	[TrPass3] [int] NOT NULL,
	[TrIndex4] [tinyint] NOT NULL,
	[TrPrice4] [int] NOT NULL,
	[TrPass4] [int] NOT NULL,
	[TrIndex5] [tinyint] NOT NULL,
	[TrPrice5] [int] NOT NULL,
	[TrPass5] [int] NOT NULL,
	[TrIndex6] [tinyint] NOT NULL,
	[TrPrice6] [int] NOT NULL,
	[TrPass6] [int] NOT NULL,
	[TrIndex7] [tinyint] NOT NULL,
	[TrPrice7] [int] NOT NULL,
	[TrPass7] [int] NOT NULL,
	[TrIndex8] [tinyint] NOT NULL,
	[TrPrice8] [int] NOT NULL,
	[TrPass8] [int] NOT NULL,
	[TrIndex9] [tinyint] NOT NULL,
	[TrPrice9] [int] NOT NULL,
	[TrPass9] [int] NOT NULL,
	[S0] [bigint] NOT NULL,
	[T0] [int] NOT NULL,
	[S1] [bigint] NOT NULL,
	[T1] [int] NOT NULL,
	[S2] [bigint] NOT NULL,
	[T2] [int] NOT NULL,
	[S3] [bigint] NOT NULL,
	[T3] [int] NOT NULL,
	[S4] [bigint] NOT NULL,
	[T4] [int] NOT NULL,
	[S5] [bigint] NOT NULL,
	[T5] [int] NOT NULL,
	[S6] [bigint] NOT NULL,
	[T6] [int] NOT NULL,
	[S7] [bigint] NOT NULL,
	[T7] [int] NOT NULL,
	[S8] [bigint] NOT NULL,
	[T8] [int] NOT NULL,
	[S9] [bigint] NOT NULL,
	[T9] [int] NOT NULL,
	[S10] [bigint] NOT NULL,
	[T10] [int] NOT NULL,
	[S11] [bigint] NOT NULL,
	[T11] [int] NOT NULL,
	[S12] [bigint] NOT NULL,
	[T12] [int] NOT NULL,
	[S13] [bigint] NOT NULL,
	[T13] [int] NOT NULL,
	[S14] [bigint] NOT NULL,
	[T14] [int] NOT NULL,
	[S15] [bigint] NOT NULL,
	[T15] [int] NOT NULL,
	[S16] [bigint] NOT NULL,
	[T16] [int] NOT NULL,
	[S17] [bigint] NOT NULL,
	[T17] [int] NOT NULL,
	[S18] [bigint] NOT NULL,
	[T18] [int] NOT NULL,
	[S19] [bigint] NOT NULL,
	[T19] [int] NOT NULL,
	[S20] [bigint] NOT NULL,
	[T20] [int] NOT NULL,
	[S21] [bigint] NOT NULL,
	[T21] [int] NOT NULL,
	[S22] [bigint] NOT NULL,
	[T22] [int] NOT NULL,
	[S23] [bigint] NOT NULL,
	[T23] [int] NOT NULL,
	[S24] [bigint] NOT NULL,
	[T24] [int] NOT NULL,
	[S25] [bigint] NOT NULL,
	[T25] [int] NOT NULL,
	[S26] [bigint] NOT NULL,
	[T26] [int] NOT NULL,
	[S27] [bigint] NOT NULL,
	[T27] [int] NOT NULL,
	[S28] [bigint] NOT NULL,
	[T28] [int] NOT NULL,
	[S29] [bigint] NOT NULL,
	[T29] [int] NOT NULL,
	[S30] [bigint] NOT NULL,
	[T30] [int] NOT NULL,
	[S31] [bigint] NOT NULL,
	[T31] [int] NOT NULL,
	[S32] [bigint] NOT NULL,
	[T32] [int] NOT NULL,
	[S33] [bigint] NOT NULL,
	[T33] [int] NOT NULL,
	[S34] [bigint] NOT NULL,
	[T34] [int] NOT NULL,
	[S35] [bigint] NOT NULL,
	[T35] [int] NOT NULL,
	[S36] [bigint] NOT NULL,
	[T36] [int] NOT NULL,
	[S37] [bigint] NOT NULL,
	[T37] [int] NOT NULL,
	[S38] [bigint] NOT NULL,
	[T38] [int] NOT NULL,
	[S39] [bigint] NOT NULL,
	[T39] [int] NOT NULL,
	[S40] [bigint] NOT NULL,
	[T40] [int] NOT NULL,
	[S41] [bigint] NOT NULL,
	[T41] [int] NOT NULL,
	[S42] [bigint] NOT NULL,
	[T42] [int] NOT NULL,
	[S43] [bigint] NOT NULL,
	[T43] [int] NOT NULL,
	[S44] [bigint] NOT NULL,
	[T44] [int] NOT NULL,
	[S45] [bigint] NOT NULL,
	[T45] [int] NOT NULL,
	[S46] [bigint] NOT NULL,
	[T46] [int] NOT NULL,
	[S47] [bigint] NOT NULL,
	[T47] [int] NOT NULL,
	[S48] [bigint] NOT NULL,
	[T48] [int] NOT NULL,
	[S49] [bigint] NOT NULL,
	[T49] [int] NOT NULL,
	[S50] [bigint] NOT NULL,
	[T50] [int] NOT NULL,
	[S51] [bigint] NOT NULL,
	[T51] [int] NOT NULL,
	[S52] [bigint] NOT NULL,
	[T52] [int] NOT NULL,
	[S53] [bigint] NOT NULL,
	[T53] [int] NOT NULL,
	[S54] [bigint] NOT NULL,
	[T54] [int] NOT NULL,
	[S55] [bigint] NOT NULL,
	[T55] [int] NOT NULL,
	[S56] [bigint] NOT NULL,
	[T56] [int] NOT NULL,
	[S57] [bigint] NOT NULL,
	[T57] [int] NOT NULL,
	[S58] [bigint] NOT NULL,
	[T58] [int] NOT NULL,
	[S59] [bigint] NOT NULL,
	[T59] [int] NOT NULL,
	[S60] [bigint] NOT NULL,
	[T60] [int] NOT NULL,
	[S61] [bigint] NOT NULL,
	[T61] [int] NOT NULL,
	[S62] [bigint] NOT NULL,
	[T62] [int] NOT NULL,
	[S63] [bigint] NOT NULL,
	[T63] [int] NOT NULL,
	[S64] [bigint] NOT NULL,
	[T64] [int] NOT NULL,
	[S65] [bigint] NOT NULL,
	[T65] [int] NOT NULL,
	[S66] [bigint] NOT NULL,
	[T66] [int] NOT NULL,
	[S67] [bigint] NOT NULL,
	[T67] [int] NOT NULL,
	[S68] [bigint] NOT NULL,
	[T68] [int] NOT NULL,
	[S69] [bigint] NOT NULL,
	[T69] [int] NOT NULL,
	[S70] [bigint] NOT NULL,
	[T70] [int] NOT NULL,
	[S71] [bigint] NOT NULL,
	[T71] [int] NOT NULL,
	[S72] [bigint] NOT NULL,
	[T72] [int] NOT NULL,
	[S73] [bigint] NOT NULL,
	[T73] [int] NOT NULL,
	[S74] [bigint] NOT NULL,
	[T74] [int] NOT NULL,
	[S75] [bigint] NOT NULL,
	[T75] [int] NOT NULL,
	[S76] [bigint] NOT NULL,
	[T76] [int] NOT NULL,
	[S77] [bigint] NOT NULL,
	[T77] [int] NOT NULL,
	[S78] [bigint] NOT NULL,
	[T78] [int] NOT NULL,
	[S79] [bigint] NOT NULL,
	[T79] [int] NOT NULL,
	[S80] [bigint] NOT NULL,
	[T80] [int] NOT NULL,
	[S81] [bigint] NOT NULL,
	[T81] [int] NOT NULL,
	[S82] [bigint] NOT NULL,
	[T82] [int] NOT NULL,
	[S83] [bigint] NOT NULL,
	[T83] [int] NOT NULL,
	[S84] [bigint] NOT NULL,
	[T84] [int] NOT NULL,
	[S85] [bigint] NOT NULL,
	[T85] [int] NOT NULL,
	[S86] [bigint] NOT NULL,
	[T86] [int] NOT NULL,
	[S87] [bigint] NOT NULL,
	[T87] [int] NOT NULL,
	[S88] [bigint] NOT NULL,
	[T88] [int] NOT NULL,
	[S89] [bigint] NOT NULL,
	[T89] [int] NOT NULL,
	[S90] [bigint] NOT NULL,
	[T90] [int] NOT NULL,
	[S91] [bigint] NOT NULL,
	[T91] [int] NOT NULL,
	[S92] [bigint] NOT NULL,
	[T92] [int] NOT NULL,
	[S93] [bigint] NOT NULL,
	[T93] [int] NOT NULL,
	[S94] [bigint] NOT NULL,
	[T94] [int] NOT NULL,
	[S95] [bigint] NOT NULL,
	[T95] [int] NOT NULL,
	[S96] [bigint] NOT NULL,
	[T96] [int] NOT NULL,
	[S97] [bigint] NOT NULL,
	[T97] [int] NOT NULL,
	[S98] [bigint] NOT NULL,
	[T98] [int] NOT NULL,
	[S99] [bigint] NOT NULL,
	[T99] [int] NOT NULL,
 CONSTRAINT [PK_tbl_inven] PRIMARY KEY CLUSTERED 
(
	[Serial] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[tbl_ItemCharge]    Script Date: 12/01/2026 16:57:19 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[tbl_ItemCharge](
	[nSerial] [int] IDENTITY(1,1) NOT NULL,
	[nAvatorSerial] [int] NOT NULL,
	[nItemCode_K] [int] NOT NULL,
	[nItemCode_D] [bigint] NOT NULL,
	[nItemCode_U] [int] NOT NULL,
	[dtGiveDate] [datetime] NOT NULL,
	[dtTakeDate] [datetime] NOT NULL,
	[DCK] [tinyint] NOT NULL,
	[Type] [tinyint] NOT NULL,
	[S] [bigint] NOT NULL,
	[T] [int] NOT NULL,
 CONSTRAINT [PK_tbl_ItemCharge] PRIMARY KEY CLUSTERED 
(
	[nSerial] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[tbl_itemcombine_ex_result]    Script Date: 12/01/2026 16:57:19 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[tbl_itemcombine_ex_result](
	[Serial] [int] NOT NULL,
	[DCK] [bit] NOT NULL,
	[IsResult] [bit] NOT NULL,
	[CheckKey] [int] NOT NULL,
	[DlgType] [tinyint] NOT NULL,
	[Dalant] [int] NOT NULL,
	[ItemListCount] [tinyint] NOT NULL,
	[ItemSelectCount] [tinyint] NOT NULL,
	[K0] [int] NOT NULL,
	[D0] [int] NOT NULL,
	[U0] [int] NOT NULL,
	[K1] [int] NOT NULL,
	[D1] [int] NOT NULL,
	[U1] [int] NOT NULL,
	[K2] [int] NOT NULL,
	[D2] [int] NOT NULL,
	[U2] [int] NOT NULL,
	[K3] [int] NOT NULL,
	[D3] [int] NOT NULL,
	[U3] [int] NOT NULL,
	[K4] [int] NOT NULL,
	[D4] [int] NOT NULL,
	[U4] [int] NOT NULL,
	[K5] [int] NOT NULL,
	[D5] [int] NOT NULL,
	[U5] [int] NOT NULL,
	[K6] [int] NOT NULL,
	[D6] [int] NOT NULL,
	[U6] [int] NOT NULL,
	[K7] [int] NOT NULL,
	[D7] [int] NOT NULL,
	[U7] [int] NOT NULL,
	[K8] [int] NOT NULL,
	[D8] [int] NOT NULL,
	[U8] [int] NOT NULL,
	[K9] [int] NOT NULL,
	[D9] [int] NOT NULL,
	[U9] [int] NOT NULL,
	[K10] [int] NOT NULL,
	[D10] [int] NOT NULL,
	[U10] [int] NOT NULL,
	[K11] [int] NOT NULL,
	[D11] [int] NOT NULL,
	[U11] [int] NOT NULL,
	[K12] [int] NOT NULL,
	[D12] [int] NOT NULL,
	[U12] [int] NOT NULL,
	[K13] [int] NOT NULL,
	[D13] [int] NOT NULL,
	[U13] [int] NOT NULL,
	[K14] [int] NOT NULL,
	[D14] [int] NOT NULL,
	[U14] [int] NOT NULL,
	[K15] [int] NOT NULL,
	[D15] [int] NOT NULL,
	[U15] [int] NOT NULL,
	[K16] [int] NOT NULL,
	[D16] [int] NOT NULL,
	[U16] [int] NOT NULL,
	[K17] [int] NOT NULL,
	[D17] [int] NOT NULL,
	[U17] [int] NOT NULL,
	[K18] [int] NOT NULL,
	[D18] [int] NOT NULL,
	[U18] [int] NOT NULL,
	[K19] [int] NOT NULL,
	[D19] [int] NOT NULL,
	[U19] [int] NOT NULL,
	[K20] [int] NOT NULL,
	[D20] [int] NOT NULL,
	[U20] [int] NOT NULL,
	[K21] [int] NOT NULL,
	[D21] [int] NOT NULL,
	[U21] [int] NOT NULL,
	[K22] [int] NOT NULL,
	[D22] [int] NOT NULL,
	[U22] [int] NOT NULL,
	[K23] [int] NOT NULL,
	[D23] [int] NOT NULL,
	[U23] [int] NOT NULL,
	[resulteffecttype] [int] NOT NULL,
	[resulteffectmsgcode] [int] NOT NULL,
 CONSTRAINT [PK_tbl_itemcombine_ex_result] PRIMARY KEY CLUSTERED 
(
	[Serial] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[tbl_leaveaccountlist]    Script Date: 12/01/2026 16:57:19 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[tbl_leaveaccountlist](
	[serial] [int] NOT NULL,
	[id] [varchar](16) NOT NULL,
 CONSTRAINT [PK_tbl_leaveaccountlist] PRIMARY KEY CLUSTERED 
(
	[serial] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[tbl_log_lv]    Script Date: 12/01/2026 16:57:19 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[tbl_log_lv](
	[AvatorSerial] [int] NOT NULL,
	[Lv] [tinyint] NOT NULL,
	[dtDate] [datetime] NOT NULL,
	[TotalMin] [int] NOT NULL,
	[Serial] [int] IDENTITY(1,1) NOT NULL,
 CONSTRAINT [PK_tbl_log_lv] PRIMARY KEY CLUSTERED 
(
	[Serial] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[tbl_log_usernum]    Script Date: 12/01/2026 16:57:19 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[tbl_log_usernum](
	[serial] [int] IDENTITY(1,1) NOT NULL,
	[dtDate] [datetime] NOT NULL,
	[nAverageUser] [int] NOT NULL,
	[nMaxUser] [int] NOT NULL,
 CONSTRAINT [PK_tbl_UserNum] PRIMARY KEY CLUSTERED 
(
	[serial] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[tbl_Macro]    Script Date: 12/01/2026 16:57:19 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[tbl_Macro](
	[serial] [int] NOT NULL,
	[belt] [tinyint] NOT NULL,
	[hp] [int] NOT NULL,
	[fp] [int] NOT NULL,
	[sp] [int] NOT NULL,
	[action0] [int] NOT NULL,
	[action1] [int] NOT NULL,
	[action2] [int] NOT NULL,
	[action3] [int] NOT NULL,
	[action4] [int] NOT NULL,
	[action5] [int] NOT NULL,
	[action6] [int] NOT NULL,
	[action7] [int] NOT NULL,
	[action8] [int] NOT NULL,
	[action9] [int] NOT NULL,
	[chat0] [varchar](255) NOT NULL,
	[chat1] [varchar](255) NOT NULL,
	[chat2] [varchar](255) NOT NULL,
	[chat3] [varchar](255) NOT NULL,
	[chat4] [varchar](255) NOT NULL,
	[hpvalue] [int] NOT NULL,
	[fpvalue] [int] NOT NULL,
	[spvalue] [int] NOT NULL
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[tbl_npc_quest_history]    Script Date: 12/01/2026 16:57:19 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[tbl_npc_quest_history](
	[Serial] [int] NOT NULL,
	[Code1] [varchar](7) NOT NULL,
	[Level1] [tinyint] NOT NULL,
	[Code2] [varchar](7) NOT NULL,
	[Level2] [tinyint] NOT NULL,
	[Code3] [varchar](7) NOT NULL,
	[Level3] [tinyint] NOT NULL,
	[Code4] [varchar](7) NOT NULL,
	[Level4] [tinyint] NOT NULL,
	[Code5] [varchar](7) NOT NULL,
	[Level5] [tinyint] NOT NULL,
	[Code6] [varchar](7) NOT NULL,
	[Level6] [tinyint] NOT NULL,
	[Code7] [varchar](7) NOT NULL,
	[Level7] [tinyint] NOT NULL,
	[Code8] [varchar](7) NOT NULL,
	[Level8] [tinyint] NOT NULL,
	[Code9] [varchar](7) NOT NULL,
	[Level9] [tinyint] NOT NULL,
	[Code10] [varchar](7) NOT NULL,
	[Level10] [tinyint] NOT NULL,
	[Code11] [varchar](7) NOT NULL,
	[Level11] [tinyint] NOT NULL,
	[Code12] [varchar](7) NOT NULL,
	[Level12] [tinyint] NOT NULL,
	[Code13] [varchar](7) NOT NULL,
	[Level13] [tinyint] NOT NULL,
	[Code14] [varchar](7) NOT NULL,
	[Level14] [tinyint] NOT NULL,
	[Code15] [varchar](7) NOT NULL,
	[Level15] [tinyint] NOT NULL,
	[Code16] [varchar](7) NOT NULL,
	[Level16] [tinyint] NOT NULL,
	[Code17] [varchar](7) NOT NULL,
	[Level17] [tinyint] NOT NULL,
	[Code18] [varchar](7) NOT NULL,
	[Level18] [tinyint] NOT NULL,
	[Code19] [varchar](7) NOT NULL,
	[Level19] [tinyint] NOT NULL,
	[Code20] [varchar](7) NOT NULL,
	[Level20] [tinyint] NOT NULL,
	[Time1] [bigint] NOT NULL,
	[Time2] [bigint] NOT NULL,
	[Time3] [bigint] NOT NULL,
	[Time4] [bigint] NOT NULL,
	[Time5] [bigint] NOT NULL,
	[Time6] [bigint] NOT NULL,
	[Time7] [bigint] NOT NULL,
	[Time8] [bigint] NOT NULL,
	[Time9] [bigint] NOT NULL,
	[Time10] [bigint] NOT NULL,
	[Time11] [bigint] NOT NULL,
	[Time12] [bigint] NOT NULL,
	[Time13] [bigint] NOT NULL,
	[Time14] [bigint] NOT NULL,
	[Time15] [bigint] NOT NULL,
	[Time16] [bigint] NOT NULL,
	[Time17] [bigint] NOT NULL,
	[Time18] [bigint] NOT NULL,
	[Time19] [bigint] NOT NULL,
	[Time20] [bigint] NOT NULL,
	[Code21] [varchar](7) NOT NULL,
	[Level21] [tinyint] NOT NULL,
	[Time21] [bigint] NOT NULL,
	[Code22] [varchar](7) NOT NULL,
	[Level22] [tinyint] NOT NULL,
	[Time22] [bigint] NOT NULL,
	[Code23] [varchar](7) NOT NULL,
	[Level23] [tinyint] NOT NULL,
	[Time23] [bigint] NOT NULL,
	[Code24] [varchar](7) NOT NULL,
	[Level24] [tinyint] NOT NULL,
	[Time24] [bigint] NOT NULL,
	[Code25] [varchar](7) NOT NULL,
	[Level25] [tinyint] NOT NULL,
	[Time25] [bigint] NOT NULL,
	[Code26] [varchar](7) NOT NULL,
	[Level26] [tinyint] NOT NULL,
	[Time26] [bigint] NOT NULL,
	[Code27] [varchar](7) NOT NULL,
	[Level27] [tinyint] NOT NULL,
	[Time27] [bigint] NOT NULL,
	[Code28] [varchar](7) NOT NULL,
	[Level28] [tinyint] NOT NULL,
	[Time28] [bigint] NOT NULL,
	[Code29] [varchar](7) NOT NULL,
	[Level29] [tinyint] NOT NULL,
	[Time29] [bigint] NOT NULL,
	[Code30] [varchar](7) NOT NULL,
	[Level30] [tinyint] NOT NULL,
	[Time30] [bigint] NOT NULL,
	[Code31] [varchar](7) NOT NULL,
	[Level31] [tinyint] NOT NULL,
	[Time31] [bigint] NOT NULL,
	[Code32] [varchar](7) NOT NULL,
	[Level32] [tinyint] NOT NULL,
	[Time32] [bigint] NOT NULL,
	[Code33] [varchar](7) NOT NULL,
	[Level33] [tinyint] NOT NULL,
	[Time33] [bigint] NOT NULL,
	[Code34] [varchar](7) NOT NULL,
	[Level34] [tinyint] NOT NULL,
	[Time34] [bigint] NOT NULL,
	[Code35] [varchar](7) NOT NULL,
	[Level35] [tinyint] NOT NULL,
	[Time35] [bigint] NOT NULL,
	[Code36] [varchar](7) NOT NULL,
	[Level36] [tinyint] NOT NULL,
	[Time36] [bigint] NOT NULL,
	[Code37] [varchar](7) NOT NULL,
	[Level37] [tinyint] NOT NULL,
	[Time37] [bigint] NOT NULL,
	[Code38] [varchar](7) NOT NULL,
	[Level38] [tinyint] NOT NULL,
	[Time38] [bigint] NOT NULL,
	[Code39] [varchar](7) NOT NULL,
	[Level39] [tinyint] NOT NULL,
	[Time39] [bigint] NOT NULL,
	[Code40] [varchar](7) NOT NULL,
	[Level40] [tinyint] NOT NULL,
	[Time40] [bigint] NOT NULL,
	[Code41] [varchar](7) NOT NULL,
	[Level41] [tinyint] NOT NULL,
	[Time41] [bigint] NOT NULL,
	[Code42] [varchar](7) NOT NULL,
	[Level42] [tinyint] NOT NULL,
	[Time42] [bigint] NOT NULL,
	[Code43] [varchar](7) NOT NULL,
	[Level43] [tinyint] NOT NULL,
	[Time43] [bigint] NOT NULL,
	[Code44] [varchar](7) NOT NULL,
	[Level44] [tinyint] NOT NULL,
	[Time44] [bigint] NOT NULL,
	[Code45] [varchar](7) NOT NULL,
	[Level45] [tinyint] NOT NULL,
	[Time45] [bigint] NOT NULL,
	[Code46] [varchar](7) NOT NULL,
	[Level46] [tinyint] NOT NULL,
	[Time46] [bigint] NOT NULL,
	[Code47] [varchar](7) NOT NULL,
	[Level47] [tinyint] NOT NULL,
	[Time47] [bigint] NOT NULL,
	[Code48] [varchar](7) NOT NULL,
	[Level48] [tinyint] NOT NULL,
	[Time48] [bigint] NOT NULL,
	[Code49] [varchar](7) NOT NULL,
	[Level49] [tinyint] NOT NULL,
	[Time49] [bigint] NOT NULL,
	[Code50] [varchar](7) NOT NULL,
	[Level50] [tinyint] NOT NULL,
	[Time50] [bigint] NOT NULL,
	[Code51] [varchar](7) NOT NULL,
	[Level51] [tinyint] NOT NULL,
	[Time51] [bigint] NOT NULL,
	[Code52] [varchar](7) NOT NULL,
	[Level52] [tinyint] NOT NULL,
	[Time52] [bigint] NOT NULL,
	[Code53] [varchar](7) NOT NULL,
	[Level53] [tinyint] NOT NULL,
	[Time53] [bigint] NOT NULL,
	[Code54] [varchar](7) NOT NULL,
	[Level54] [tinyint] NOT NULL,
	[Time54] [bigint] NOT NULL,
	[Code55] [varchar](7) NOT NULL,
	[Level55] [tinyint] NOT NULL,
	[Time55] [bigint] NOT NULL,
	[Code56] [varchar](7) NOT NULL,
	[Level56] [tinyint] NOT NULL,
	[Time56] [bigint] NOT NULL,
	[Code57] [varchar](7) NOT NULL,
	[Level57] [tinyint] NOT NULL,
	[Time57] [bigint] NOT NULL,
	[Code58] [varchar](7) NOT NULL,
	[Level58] [tinyint] NOT NULL,
	[Time58] [bigint] NOT NULL,
	[Code59] [varchar](7) NOT NULL,
	[Level59] [tinyint] NOT NULL,
	[Time59] [bigint] NOT NULL,
	[Code60] [varchar](7) NOT NULL,
	[Level60] [tinyint] NOT NULL,
	[Time60] [bigint] NOT NULL,
	[Code61] [varchar](7) NOT NULL,
	[Level61] [tinyint] NOT NULL,
	[Time61] [bigint] NOT NULL,
	[Code62] [varchar](7) NOT NULL,
	[Level62] [tinyint] NOT NULL,
	[Time62] [bigint] NOT NULL,
	[Code63] [varchar](7) NOT NULL,
	[Level63] [tinyint] NOT NULL,
	[Time63] [bigint] NOT NULL,
	[Code64] [varchar](7) NOT NULL,
	[Level64] [tinyint] NOT NULL,
	[Time64] [bigint] NOT NULL,
	[Code65] [varchar](7) NOT NULL,
	[Level65] [tinyint] NOT NULL,
	[Time65] [bigint] NOT NULL,
	[Code66] [varchar](7) NOT NULL,
	[Level66] [tinyint] NOT NULL,
	[Time66] [bigint] NOT NULL,
	[Code67] [varchar](7) NOT NULL,
	[Level67] [tinyint] NOT NULL,
	[Time67] [bigint] NOT NULL,
	[Code68] [varchar](7) NOT NULL,
	[Level68] [tinyint] NOT NULL,
	[Time68] [bigint] NOT NULL,
	[Code69] [varchar](7) NOT NULL,
	[Level69] [tinyint] NOT NULL,
	[Time69] [bigint] NOT NULL,
	[Code70] [varchar](7) NOT NULL,
	[Level70] [tinyint] NOT NULL,
	[Time70] [bigint] NOT NULL,
 CONSTRAINT [PK_tbl_npc_quest] PRIMARY KEY CLUSTERED 
(
	[Serial] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[tbl_NpcData]    Script Date: 12/01/2026 16:57:19 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[tbl_NpcData](
	[Serial] [int] NOT NULL,
	[Npc0] [int] NOT NULL,
	[Npc1] [int] NOT NULL,
	[Npc2] [int] NOT NULL,
	[Npc3] [int] NOT NULL,
	[Npc4] [int] NOT NULL,
	[Npc5] [int] NOT NULL,
	[Npc6] [int] NOT NULL,
	[Npc7] [int] NOT NULL,
	[Npc8] [int] NOT NULL,
	[Npc9] [int] NOT NULL,
 CONSTRAINT [PK_tbl_NpcData] PRIMARY KEY CLUSTERED 
(
	[Serial] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[tbl_NpcLog]    Script Date: 12/01/2026 16:57:19 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[tbl_NpcLog](
	[idx] [int] IDENTITY(1,1) NOT NULL,
	[CharSerial] [int] NOT NULL,
	[Name] [varchar](17) NOT NULL,
	[ValueIndex] [smallint] NOT NULL,
	[OrgValue] [int] NOT NULL,
	[ChangeValue] [int] NOT NULL,
	[LogDate] [datetime] NOT NULL,
 CONSTRAINT [PK_tbl_NpcLog] PRIMARY KEY CLUSTERED 
(
	[idx] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[tbl_OreCutting]    Script Date: 12/01/2026 16:57:19 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[tbl_OreCutting](
	[Serial] [int] NOT NULL,
	[DCK] [bit] NOT NULL,
	[K0] [int] NOT NULL,
	[D0] [int] NOT NULL,
	[K1] [int] NOT NULL,
	[D1] [int] NOT NULL,
	[K2] [int] NOT NULL,
	[D2] [int] NOT NULL,
	[K3] [int] NOT NULL,
	[D3] [int] NOT NULL,
	[K4] [int] NOT NULL,
	[D4] [int] NOT NULL,
	[K5] [int] NOT NULL,
	[D5] [int] NOT NULL,
	[K6] [int] NOT NULL,
	[D6] [int] NOT NULL,
	[K7] [int] NOT NULL,
	[D7] [int] NOT NULL,
	[K8] [int] NOT NULL,
	[D8] [int] NOT NULL,
	[K9] [int] NOT NULL,
	[D9] [int] NOT NULL,
	[K10] [int] NOT NULL,
	[D10] [int] NOT NULL,
	[K11] [int] NOT NULL,
	[D11] [int] NOT NULL,
	[K12] [int] NOT NULL,
	[D12] [int] NOT NULL,
	[K13] [int] NOT NULL,
	[D13] [int] NOT NULL,
	[K14] [int] NOT NULL,
	[D14] [int] NOT NULL,
	[K15] [int] NOT NULL,
	[D15] [int] NOT NULL,
	[K16] [int] NOT NULL,
	[D16] [int] NOT NULL,
	[K17] [int] NOT NULL,
	[D17] [int] NOT NULL,
	[K18] [int] NOT NULL,
	[D18] [int] NOT NULL,
	[K19] [int] NOT NULL,
	[D19] [int] NOT NULL,
 CONSTRAINT [PK__tbl_OreCutting] PRIMARY KEY CLUSTERED 
(
	[Serial] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[tbl_patriarch_candidate]    Script Date: 12/01/2026 16:57:19 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[tbl_patriarch_candidate](
	[Serial] [int] IDENTITY(1,1) NOT NULL,
	[eSerial] [int] NOT NULL,
	[Race] [tinyint] NOT NULL,
	[Lv] [tinyint] NOT NULL,
	[Rank] [int] NOT NULL,
	[Grade] [tinyint] NOT NULL,
	[PvpPoint] [float] NOT NULL,
	[ASerial] [int] NOT NULL,
	[AName] [nvarchar](17) NOT NULL,
	[GSerial] [int] NOT NULL,
	[GName] [nvarchar](17) NOT NULL,
	[WinCnt] [int] NOT NULL,
	[ClassType] [tinyint] NOT NULL,
	[State] [tinyint] NOT NULL,
	[Score] [int] NOT NULL,
	[Refund] [tinyint] NOT NULL,
	[dtUpScore] [datetime] NOT NULL,
 CONSTRAINT [PK_tbl_patriarch_candidate] PRIMARY KEY CLUSTERED 
(
	[Serial] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[tbl_patriarch_comm]    Script Date: 12/01/2026 16:57:19 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[tbl_patriarch_comm](
	[idx] [int] IDENTITY(1,1) NOT NULL,
	[AvatorSerial] [int] NOT NULL,
	[Dalant] [int] NOT NULL,
	[DepositDate] [char](8) NOT NULL,
	[PayDate] [datetime] NOT NULL,
	[DCK] [tinyint] NOT NULL,
 CONSTRAINT [PK_tbl_patriarch_comm] PRIMARY KEY CLUSTERED 
(
	[idx] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[tbl_patriarch_elect]    Script Date: 12/01/2026 16:57:19 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[tbl_patriarch_elect](
	[Serial] [int] IDENTITY(1,1) NOT NULL,
	[WorldName] [varchar](32) NOT NULL,
	[ProcType] [tinyint] NOT NULL,
	[NonVoteCnt_0] [int] NOT NULL,
	[NonVoteCnt_1] [int] NOT NULL,
	[NonVoteCnt_2] [int] NOT NULL,
	[VoteCnt_0] [int] NOT NULL,
	[VoteCnt_1] [int] NOT NULL,
	[VoteCnt_2] [int] NOT NULL,
	[tmCreate] [datetime] NOT NULL,
	[tmUpdate] [datetime] NOT NULL,
	[HighGradeNum_0] [int] NOT NULL,
	[HighGradeNum_1] [int] NOT NULL,
	[HighGradeNum_2] [int] NOT NULL,
	[VoteInfoResetTime] [int] NOT NULL,
 CONSTRAINT [PK_tbl_patriarch_elect] PRIMARY KEY CLUSTERED 
(
	[Serial] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[tbl_pcbangitem]    Script Date: 12/01/2026 16:57:19 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[tbl_pcbangitem](
	[Serial] [int] NOT NULL,
	[DCK] [tinyint] NOT NULL,
	[K0] [bigint] NOT NULL,
	[K1] [bigint] NOT NULL,
	[K2] [bigint] NOT NULL,
	[K3] [bigint] NOT NULL,
	[K4] [bigint] NOT NULL,
	[K5] [bigint] NOT NULL,
	[K6] [bigint] NOT NULL,
	[K7] [bigint] NOT NULL,
	[K8] [bigint] NOT NULL,
	[K9] [bigint] NOT NULL,
	[K10] [bigint] NOT NULL,
	[K11] [bigint] NOT NULL,
	[K12] [bigint] NOT NULL,
	[K13] [bigint] NOT NULL,
	[K14] [bigint] NOT NULL,
	[K15] [bigint] NOT NULL,
	[K16] [bigint] NOT NULL,
	[K17] [bigint] NOT NULL,
	[K18] [bigint] NOT NULL,
	[K19] [bigint] NOT NULL,
	[K20] [bigint] NOT NULL,
	[K21] [bigint] NOT NULL,
	[K22] [bigint] NOT NULL,
	[K23] [bigint] NOT NULL,
	[K24] [bigint] NOT NULL,
	[K25] [bigint] NOT NULL,
	[K26] [bigint] NOT NULL,
	[K27] [bigint] NOT NULL,
	[K28] [bigint] NOT NULL,
	[K29] [bigint] NOT NULL,
	[K30] [bigint] NOT NULL,
	[K31] [bigint] NOT NULL,
	[K32] [bigint] NOT NULL,
	[K33] [bigint] NOT NULL,
	[K34] [bigint] NOT NULL,
	[K35] [bigint] NOT NULL,
	[K36] [bigint] NOT NULL,
	[K37] [bigint] NOT NULL,
	[K38] [bigint] NOT NULL,
	[K39] [bigint] NOT NULL,
	[K40] [bigint] NOT NULL,
	[K41] [bigint] NOT NULL,
	[K42] [bigint] NOT NULL,
	[K43] [bigint] NOT NULL,
	[K44] [bigint] NOT NULL,
	[K45] [bigint] NOT NULL,
	[K46] [bigint] NOT NULL,
	[K47] [bigint] NOT NULL,
	[K48] [bigint] NOT NULL,
	[K49] [bigint] NOT NULL,
 CONSTRAINT [PK_tbl_pcbangitem] PRIMARY KEY CLUSTERED 
(
	[Serial] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[tbl_PostRegistry]    Script Date: 12/01/2026 16:57:19 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[tbl_PostRegistry](
	[serial] [int] IDENTITY(0,1) NOT NULL,
	[dck] [bit] NOT NULL,
	[sendserial] [int] NOT NULL,
	[sendname] [varchar](17) NULL,
	[recvname] [varchar](17) NULL,
	[title] [varchar](21) NULL,
	[content] [varchar](201) NULL,
	[k] [int] NOT NULL,
	[d] [bigint] NOT NULL,
	[u] [int] NOT NULL,
	[gold] [int] NOT NULL,
	[sendrace] [smallint] NOT NULL,
	[userdgr] [smallint] NOT NULL,
	[uid] [bigint] NOT NULL,
PRIMARY KEY CLUSTERED 
(
	[serial] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[tbl_PostStorage]    Script Date: 12/01/2026 16:57:19 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[tbl_PostStorage](
	[serial] [int] IDENTITY(1,1) NOT NULL,
	[postinx] [tinyint] NOT NULL,
	[owner] [int] NOT NULL,
	[dck] [bit] NOT NULL,
	[poststate] [tinyint] NOT NULL,
	[sendname] [varchar](17) NULL,
	[recvname] [varchar](17) NULL,
	[title] [varchar](21) NULL,
	[content] [varchar](201) NULL,
	[k] [int] NOT NULL,
	[d] [bigint] NOT NULL,
	[u] [int] NOT NULL,
	[gold] [int] NOT NULL,
	[err] [tinyint] NOT NULL,
	[uid] [bigint] NOT NULL,
	[sindex] [smallint] NOT NULL,
PRIMARY KEY CLUSTERED 
(
	[serial] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[tbl_potion_delay]    Script Date: 12/01/2026 16:57:19 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[tbl_potion_delay](
	[Serial] [int] NOT NULL,
	[DCK] [bit] NOT NULL,
	[PD0] [int] NOT NULL,
	[PD1] [int] NOT NULL,
	[PD2] [int] NOT NULL,
	[PD3] [int] NOT NULL,
	[PD4] [int] NOT NULL,
	[PD5] [int] NOT NULL,
	[PD6] [int] NOT NULL,
	[PD7] [int] NOT NULL,
	[PD8] [int] NOT NULL,
	[PD9] [int] NOT NULL,
	[PD10] [int] NOT NULL,
	[PD11] [int] NOT NULL,
	[PD12] [int] NOT NULL,
	[PD13] [int] NOT NULL,
	[PD14] [int] NOT NULL,
	[PD15] [int] NOT NULL,
	[PD16] [int] NOT NULL,
	[PD17] [int] NOT NULL,
	[PD18] [int] NOT NULL,
	[PD19] [int] NOT NULL,
	[PD20] [int] NOT NULL,
	[PD21] [int] NOT NULL,
	[PD22] [int] NOT NULL,
	[PD23] [int] NOT NULL,
	[PD24] [int] NOT NULL,
	[PD25] [int] NOT NULL,
	[PD26] [int] NOT NULL,
	[PD27] [int] NOT NULL,
	[PD28] [int] NOT NULL,
	[PD33] [int] NOT NULL,
	[PD34] [int] NOT NULL,
	[PD35] [int] NOT NULL,
	[PD36] [int] NOT NULL,
	[PD37] [int] NOT NULL,
PRIMARY KEY CLUSTERED 
(
	[Serial] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[tbl_PrimiumPlayTime]    Script Date: 12/01/2026 16:57:19 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[tbl_PrimiumPlayTime](
	[AccountSerial] [int] NOT NULL,
	[LastConnTime] [bigint] NOT NULL,
	[ContPlayTime] [int] NOT NULL,
	[ForcedClose] [bit] NOT NULL,
	[ReceiveCoupon] [tinyint] NOT NULL,
	[EnsureTime] [tinyint] NOT NULL,
 CONSTRAINT [PK_tbl_PrimiumPlayTime] PRIMARY KEY CLUSTERED 
(
	[AccountSerial] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[tbl_Punishment]    Script Date: 12/01/2026 16:57:19 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[tbl_Punishment](
	[ASerial] [int] NOT NULL,
	[ESerial] [int] NOT NULL,
	[Type] [tinyint] NOT NULL,
	[LTime] [int] NOT NULL
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[tbl_pvporderview]    Script Date: 12/01/2026 16:57:19 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[tbl_pvporderview](
	[serial] [int] NOT NULL,
	[UpdateDate] [datetime] NOT NULL,
	[Death] [int] NOT NULL,
	[Kill] [int] NOT NULL,
	[TodayStacked] [float] NOT NULL,
	[PvpPoint] [float] NOT NULL,
	[PvpTempCash] [float] NOT NULL,
	[PvpCash] [float] NOT NULL,
	[KillerSerial0] [int] NOT NULL,
	[KillerSerial1] [int] NOT NULL,
	[KillerSerial2] [int] NOT NULL,
	[KillerSerial3] [int] NOT NULL,
	[KillerSerial4] [int] NOT NULL,
	[KillerSerial5] [int] NOT NULL,
	[KillerSerial6] [int] NOT NULL,
	[KillerSerial7] [int] NOT NULL,
	[KillerSerial8] [int] NOT NULL,
	[KillerSerial9] [int] NOT NULL,
	[ContHaveCash] [tinyint] NOT NULL,
	[ContLoseCash] [tinyint] NOT NULL,
	[RaceWarRecvr] [bit] NOT NULL,
 CONSTRAINT [PK_tbl_pvporderview] PRIMARY KEY CLUSTERED 
(
	[serial] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[tbl_pvppointlimitinfo]    Script Date: 12/01/2026 16:57:19 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[tbl_pvppointlimitinfo](
	[serial] [int] NOT NULL,
	[dck] [bit] NOT NULL,
	[useup] [bit] NOT NULL,
	[updatedate] [datetime] NOT NULL,
	[limitrate] [tinyint] NOT NULL,
	[originalpoint] [float] NOT NULL,
	[limitpoint] [float] NOT NULL,
	[usepoint] [float] NOT NULL,
 CONSTRAINT [PK_tbl_pvppointlimitinfo_serial] PRIMARY KEY CLUSTERED 
(
	[serial] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[tbl_PvpRankToday]    Script Date: 12/01/2026 16:57:19 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[tbl_PvpRankToday](
	[Rank] [int] NOT NULL,
	[Rate] [int] NOT NULL,
	[serial] [int] NOT NULL,
	[name] [varchar](17) NOT NULL,
	[lv] [int] NOT NULL,
	[race] [int] NOT NULL,
	[PvpPoint] [float] NOT NULL,
	[Grade] [smallint] NOT NULL,
	[GuildSerial] [int] NOT NULL,
	[GuildName] [varchar](17) NOT NULL
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[tbl_quest]    Script Date: 12/01/2026 16:57:19 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[tbl_quest](
	[Serial] [int] NOT NULL,
	[Type0] [tinyint] NOT NULL,
	[QuestIndex0] [smallint] NOT NULL,
	[FirstAct0] [smallint] NOT NULL,
	[SecondAct0] [smallint] NOT NULL,
	[ThirdAct0] [smallint] NOT NULL,
	[PassSec0] [int] NOT NULL,
	[Type1] [tinyint] NOT NULL,
	[QuestIndex1] [smallint] NOT NULL,
	[FirstAct1] [smallint] NOT NULL,
	[SecondAct1] [smallint] NOT NULL,
	[ThirdAct1] [smallint] NOT NULL,
	[PassSec1] [int] NOT NULL,
	[Type2] [tinyint] NOT NULL,
	[QuestIndex2] [smallint] NOT NULL,
	[FirstAct2] [smallint] NOT NULL,
	[SecondAct2] [smallint] NOT NULL,
	[ThirdAct2] [smallint] NOT NULL,
	[PassSec2] [int] NOT NULL,
	[Type3] [tinyint] NOT NULL,
	[QuestIndex3] [smallint] NOT NULL,
	[FirstAct3] [smallint] NOT NULL,
	[SecondAct3] [smallint] NOT NULL,
	[ThirdAct3] [smallint] NOT NULL,
	[PassSec3] [int] NOT NULL,
	[Type4] [tinyint] NOT NULL,
	[QuestIndex4] [smallint] NOT NULL,
	[FirstAct4] [smallint] NOT NULL,
	[SecondAct4] [smallint] NOT NULL,
	[ThirdAct4] [smallint] NOT NULL,
	[PassSec4] [int] NOT NULL,
	[Type5] [tinyint] NOT NULL,
	[QuestIndex5] [smallint] NOT NULL,
	[FirstAct5] [smallint] NOT NULL,
	[SecondAct5] [smallint] NOT NULL,
	[ThirdAct5] [smallint] NOT NULL,
	[PassSec5] [int] NOT NULL,
	[Type6] [tinyint] NOT NULL,
	[QuestIndex6] [smallint] NOT NULL,
	[FirstAct6] [smallint] NOT NULL,
	[SecondAct6] [smallint] NOT NULL,
	[ThirdAct6] [smallint] NOT NULL,
	[PassSec6] [int] NOT NULL,
	[Type7] [tinyint] NOT NULL,
	[QuestIndex7] [smallint] NOT NULL,
	[FirstAct7] [smallint] NOT NULL,
	[SecondAct7] [smallint] NOT NULL,
	[ThirdAct7] [smallint] NOT NULL,
	[PassSec7] [int] NOT NULL,
	[Type8] [tinyint] NOT NULL,
	[QuestIndex8] [smallint] NOT NULL,
	[FirstAct8] [smallint] NOT NULL,
	[SecondAct8] [smallint] NOT NULL,
	[ThirdAct8] [smallint] NOT NULL,
	[PassSec8] [int] NOT NULL,
	[Type9] [tinyint] NOT NULL,
	[QuestIndex9] [smallint] NOT NULL,
	[FirstAct9] [smallint] NOT NULL,
	[SecondAct9] [smallint] NOT NULL,
	[ThirdAct9] [smallint] NOT NULL,
	[PassSec9] [int] NOT NULL,
	[Type10] [tinyint] NOT NULL,
	[QuestIndex10] [smallint] NOT NULL,
	[FirstAct10] [smallint] NOT NULL,
	[SecondAct10] [smallint] NOT NULL,
	[ThirdAct10] [smallint] NOT NULL,
	[PassSec10] [int] NOT NULL,
	[Type11] [tinyint] NOT NULL,
	[QuestIndex11] [smallint] NOT NULL,
	[FirstAct11] [smallint] NOT NULL,
	[SecondAct11] [smallint] NOT NULL,
	[ThirdAct11] [smallint] NOT NULL,
	[PassSec11] [int] NOT NULL,
	[Type12] [tinyint] NOT NULL,
	[QuestIndex12] [smallint] NOT NULL,
	[FirstAct12] [smallint] NOT NULL,
	[SecondAct12] [smallint] NOT NULL,
	[ThirdAct12] [smallint] NOT NULL,
	[PassSec12] [int] NOT NULL,
	[Type13] [tinyint] NOT NULL,
	[QuestIndex13] [smallint] NOT NULL,
	[FirstAct13] [smallint] NOT NULL,
	[SecondAct13] [smallint] NOT NULL,
	[ThirdAct13] [smallint] NOT NULL,
	[PassSec13] [int] NOT NULL,
	[Type14] [tinyint] NOT NULL,
	[QuestIndex14] [smallint] NOT NULL,
	[FirstAct14] [smallint] NOT NULL,
	[SecondAct14] [smallint] NOT NULL,
	[ThirdAct14] [smallint] NOT NULL,
	[PassSec14] [int] NOT NULL,
	[DCK] [bit] NOT NULL,
	[Type15] [tinyint] NOT NULL,
	[QuestIndex15] [smallint] NOT NULL,
	[FirstAct15] [smallint] NOT NULL,
	[SecondAct15] [smallint] NOT NULL,
	[ThirdAct15] [smallint] NOT NULL,
	[PassSec15] [int] NOT NULL,
	[Type16] [tinyint] NOT NULL,
	[QuestIndex16] [smallint] NOT NULL,
	[FirstAct16] [smallint] NOT NULL,
	[SecondAct16] [smallint] NOT NULL,
	[ThirdAct16] [smallint] NOT NULL,
	[PassSec16] [int] NOT NULL,
	[Type17] [tinyint] NOT NULL,
	[QuestIndex17] [smallint] NOT NULL,
	[FirstAct17] [smallint] NOT NULL,
	[SecondAct17] [smallint] NOT NULL,
	[ThirdAct17] [smallint] NOT NULL,
	[PassSec17] [int] NOT NULL,
	[Type18] [tinyint] NOT NULL,
	[QuestIndex18] [smallint] NOT NULL,
	[FirstAct18] [smallint] NOT NULL,
	[SecondAct18] [smallint] NOT NULL,
	[ThirdAct18] [smallint] NOT NULL,
	[PassSec18] [int] NOT NULL,
	[Type19] [tinyint] NOT NULL,
	[QuestIndex19] [smallint] NOT NULL,
	[FirstAct19] [smallint] NOT NULL,
	[SecondAct19] [smallint] NOT NULL,
	[ThirdAct19] [smallint] NOT NULL,
	[PassSec19] [int] NOT NULL,
	[Type20] [tinyint] NOT NULL,
	[QuestIndex20] [smallint] NOT NULL,
	[FirstAct20] [smallint] NOT NULL,
	[SecondAct20] [smallint] NOT NULL,
	[ThirdAct20] [smallint] NOT NULL,
	[PassSec20] [int] NOT NULL,
	[Type21] [tinyint] NOT NULL,
	[QuestIndex21] [smallint] NOT NULL,
	[FirstAct21] [smallint] NOT NULL,
	[SecondAct21] [smallint] NOT NULL,
	[ThirdAct21] [smallint] NOT NULL,
	[PassSec21] [int] NOT NULL,
	[Type22] [tinyint] NOT NULL,
	[QuestIndex22] [smallint] NOT NULL,
	[FirstAct22] [smallint] NOT NULL,
	[SecondAct22] [smallint] NOT NULL,
	[ThirdAct22] [smallint] NOT NULL,
	[PassSec22] [int] NOT NULL,
	[Type23] [tinyint] NOT NULL,
	[QuestIndex23] [smallint] NOT NULL,
	[FirstAct23] [smallint] NOT NULL,
	[SecondAct23] [smallint] NOT NULL,
	[ThirdAct23] [smallint] NOT NULL,
	[PassSec23] [int] NOT NULL,
	[Type24] [tinyint] NOT NULL,
	[QuestIndex24] [smallint] NOT NULL,
	[FirstAct24] [smallint] NOT NULL,
	[SecondAct24] [smallint] NOT NULL,
	[ThirdAct24] [smallint] NOT NULL,
	[PassSec24] [int] NOT NULL,
	[Type25] [tinyint] NOT NULL,
	[QuestIndex25] [smallint] NOT NULL,
	[FirstAct25] [smallint] NOT NULL,
	[SecondAct25] [smallint] NOT NULL,
	[ThirdAct25] [smallint] NOT NULL,
	[PassSec25] [int] NOT NULL,
	[Type26] [tinyint] NOT NULL,
	[QuestIndex26] [smallint] NOT NULL,
	[FirstAct26] [smallint] NOT NULL,
	[SecondAct26] [smallint] NOT NULL,
	[ThirdAct26] [smallint] NOT NULL,
	[PassSec26] [int] NOT NULL,
	[Type27] [tinyint] NOT NULL,
	[QuestIndex27] [smallint] NOT NULL,
	[FirstAct27] [smallint] NOT NULL,
	[SecondAct27] [smallint] NOT NULL,
	[ThirdAct27] [smallint] NOT NULL,
	[PassSec27] [int] NOT NULL,
	[Type28] [tinyint] NOT NULL,
	[QuestIndex28] [smallint] NOT NULL,
	[FirstAct28] [smallint] NOT NULL,
	[SecondAct28] [smallint] NOT NULL,
	[ThirdAct28] [smallint] NOT NULL,
	[PassSec28] [int] NOT NULL,
	[Type29] [tinyint] NOT NULL,
	[QuestIndex29] [smallint] NOT NULL,
	[FirstAct29] [smallint] NOT NULL,
	[SecondAct29] [smallint] NOT NULL,
	[ThirdAct29] [smallint] NOT NULL,
	[PassSec29] [int] NOT NULL,
 CONSTRAINT [PK_tbl_quest] PRIMARY KEY CLUSTERED 
(
	[Serial] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[tbl_racebattle_log]    Script Date: 12/01/2026 16:57:19 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[tbl_racebattle_log](
	[idx] [int] IDENTITY(1,1) NOT NULL,
	[worldname] [varchar](32) NOT NULL,
	[szdate] [char](8) NOT NULL,
	[nth] [tinyint] NOT NULL,
	[endtime] [int] NOT NULL,
	[winrace] [tinyint] NOT NULL,
	[loserace] [tinyint] NOT NULL,
	[regdate] [datetime] NOT NULL,
	[bossserial0] [int] NOT NULL,
	[bossserial1] [int] NOT NULL,
	[bossserial2] [int] NOT NULL,
 CONSTRAINT [PK_tbl_racebattle_log] PRIMARY KEY CLUSTERED 
(
	[idx] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[tbl_RemainOre_Log]    Script Date: 12/01/2026 16:57:19 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[tbl_RemainOre_Log](
	[LogType] [tinyint] NOT NULL,
	[logDate] [datetime] NOT NULL,
	[livenum] [int] NOT NULL,
	[remain] [int] NOT NULL,
	[totalAmount] [int] NOT NULL
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[tbl_RenamePotion_Log]    Script Date: 12/01/2026 16:57:19 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[tbl_RenamePotion_Log](
	[Idx] [int] IDENTITY(1,1) NOT NULL,
	[Serial] [int] NOT NULL,
	[OldName] [varchar](17) NOT NULL,
	[NewName] [varchar](17) NOT NULL,
 CONSTRAINT [PK__tbl_RenamePotion__77A0CD21] PRIMARY KEY CLUSTERED 
(
	[Idx] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[tbl_ReservedGuildBattleInfo]    Script Date: 12/01/2026 16:57:19 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[tbl_ReservedGuildBattleInfo](
	[ID] [int] NOT NULL,
	[P1GuildSerial] [int] NOT NULL,
	[P2GuildSerial] [int] NOT NULL,
	[MapID] [int] NOT NULL,
	[Number] [tinyint] NOT NULL,
 CONSTRAINT [PK_tbl_ReservedGuildBattleInfo] PRIMARY KEY CLUSTERED 
(
	[ID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[tbl_settlementownerlog]    Script Date: 12/01/2026 16:57:19 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[tbl_settlementownerlog](
	[serial] [int] IDENTITY(1,1) NOT NULL,
	[owndate] [smalldatetime] NOT NULL,
	[nth] [tinyint] NOT NULL,
	[race] [tinyint] NOT NULL,
	[guildserial] [int] NOT NULL,
	[guildname] [varchar](17) NOT NULL,
	[pvprank] [int] NOT NULL,
	[guildgrade] [tinyint] NOT NULL,
	[killpvppointsum] [float] NOT NULL,
	[battlepvppoinsum] [float] NOT NULL,
	[guildmembersumlv] [int] NOT NULL,
 CONSTRAINT [PK_tbl_settlementownerlog_serial] PRIMARY KEY CLUSTERED 
(
	[owndate] ASC,
	[nth] ASC,
	[race] ASC,
	[serial] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[tbl_sf_delay]    Script Date: 12/01/2026 16:57:19 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[tbl_sf_delay](
	[aserial] [int] NOT NULL,
	[effect] [binary](130) NULL,
 CONSTRAINT [PK_tbl_sf_delay] PRIMARY KEY CLUSTERED 
(
	[aserial] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[tbl_start_npc_quest_history]    Script Date: 12/01/2026 16:57:19 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[tbl_start_npc_quest_history](
	[serial] [int] NOT NULL,
	[code] [varchar](64) NOT NULL,
	[level] [tinyint] NOT NULL,
	[startdate] [datetime] NOT NULL,
	[EndTime] [bigint] NOT NULL,
 CONSTRAINT [PK_tbl_start_npc_quest_history] PRIMARY KEY CLUSTERED 
(
	[serial] ASC,
	[code] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[tbl_StoreLimitItem_061212]    Script Date: 12/01/2026 16:57:19 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[tbl_StoreLimitItem_061212](
	[serial] [int] IDENTITY(1,1) NOT NULL,
	[type] [smallint] NOT NULL,
	[typeserial] [int] NOT NULL,
	[storeinx] [int] NOT NULL,
	[dck] [bit] NOT NULL,
	[k0] [int] NOT NULL,
	[num0] [smallint] NOT NULL,
	[k1] [int] NOT NULL,
	[num1] [smallint] NOT NULL,
	[k2] [int] NOT NULL,
	[num2] [smallint] NOT NULL,
	[k3] [int] NOT NULL,
	[num3] [smallint] NOT NULL,
	[k4] [int] NOT NULL,
	[num4] [smallint] NOT NULL,
	[k5] [int] NOT NULL,
	[num5] [smallint] NOT NULL,
	[k6] [int] NOT NULL,
	[num6] [smallint] NOT NULL,
	[k7] [int] NOT NULL,
	[num7] [smallint] NOT NULL,
	[k8] [int] NOT NULL,
	[num8] [smallint] NOT NULL,
	[k9] [int] NOT NULL,
	[num9] [smallint] NOT NULL,
	[k10] [int] NOT NULL,
	[num10] [smallint] NOT NULL,
	[k11] [int] NOT NULL,
	[num11] [smallint] NOT NULL,
	[k12] [int] NOT NULL,
	[num12] [smallint] NOT NULL,
	[k13] [int] NOT NULL,
	[num13] [smallint] NOT NULL,
	[k14] [int] NOT NULL,
	[num14] [smallint] NOT NULL,
	[k15] [int] NOT NULL,
	[num15] [smallint] NOT NULL,
	[resettime] [bigint] NOT NULL,
PRIMARY KEY CLUSTERED 
(
	[serial] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[tbl_supplement]    Script Date: 12/01/2026 16:57:19 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[tbl_supplement](
	[Serial] [int] NOT NULL,
	[DCK] [bit] NOT NULL,
	[PvpPointLeak] [float] NOT NULL,
	[LastAttBuff] [bit] NOT NULL,
	[cristalbattledate] [int] NOT NULL,
	[BufEndTime] [bigint] NOT NULL,
	[RaceBuffClear] [int] NULL,
	[ScanerCnt] [bigint] NOT NULL,
	[AccumPlayTime] [int] NOT NULL,
	[IsVoted] [int] NOT NULL,
	[VoteEnable] [int] NOT NULL,
	[ResetAccumPlayTime] [int] NOT NULL,
	[ActionPoint_0] [int] NOT NULL,
	[ActionPoint_1] [int] NOT NULL,
	[ActionPoint_2] [int] NOT NULL,
	[EK6] [int] NOT NULL,
	[ED6] [smallint] NOT NULL,
	[ES6] [bigint] NOT NULL,
	[ET6] [int] NOT NULL,
PRIMARY KEY CLUSTERED 
(
	[Serial] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[tbl_TimeLimitInfo]    Script Date: 12/01/2026 16:57:19 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[tbl_TimeLimitInfo](
	[AccountSerial] [int] NOT NULL,
	[TLStatus] [smallint] NOT NULL,
	[Fatigue] [int] NOT NULL,
	[LastLogoutTime] [bigint] NOT NULL,
 CONSTRAINT [PK_tbl_TimeLimitInfo] PRIMARY KEY CLUSTERED 
(
	[AccountSerial] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[tbl_unit]    Script Date: 12/01/2026 16:57:19 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[tbl_unit](
	[Serial] [int] NOT NULL,
	[DCK] [bit] NOT NULL,
	[F_0] [tinyint] NOT NULL,
	[H_0] [tinyint] NOT NULL,
	[U_0] [tinyint] NOT NULL,
	[L_0] [tinyint] NOT NULL,
	[A_0] [tinyint] NOT NULL,
	[S_0] [tinyint] NOT NULL,
	[B_0] [tinyint] NOT NULL,
	[Gg_0] [int] NOT NULL,
	[AB_0] [int] NOT NULL,
	[SB_0] [int] NOT NULL,
	[Sp0_0] [int] NOT NULL,
	[Sp1_0] [int] NOT NULL,
	[Sp2_0] [int] NOT NULL,
	[Sp3_0] [int] NOT NULL,
	[Sp4_0] [int] NOT NULL,
	[Sp5_0] [int] NOT NULL,
	[Sp6_0] [int] NOT NULL,
	[Sp7_0] [int] NOT NULL,
	[KF_0] [int] NOT NULL,
	[PF_0] [int] NOT NULL,
	[Cut_0] [bigint] NOT NULL,
	[F_1] [tinyint] NOT NULL,
	[H_1] [tinyint] NOT NULL,
	[U_1] [tinyint] NOT NULL,
	[L_1] [tinyint] NOT NULL,
	[A_1] [tinyint] NOT NULL,
	[S_1] [tinyint] NOT NULL,
	[B_1] [tinyint] NOT NULL,
	[Gg_1] [int] NOT NULL,
	[AB_1] [int] NOT NULL,
	[SB_1] [int] NOT NULL,
	[Sp0_1] [int] NOT NULL,
	[Sp1_1] [int] NOT NULL,
	[Sp2_1] [int] NOT NULL,
	[Sp3_1] [int] NOT NULL,
	[Sp4_1] [int] NOT NULL,
	[Sp5_1] [int] NOT NULL,
	[Sp6_1] [int] NOT NULL,
	[Sp7_1] [int] NOT NULL,
	[KF_1] [int] NOT NULL,
	[PF_1] [int] NOT NULL,
	[Cut_1] [bigint] NOT NULL,
	[F_2] [tinyint] NOT NULL,
	[H_2] [tinyint] NOT NULL,
	[U_2] [tinyint] NOT NULL,
	[L_2] [tinyint] NOT NULL,
	[A_2] [tinyint] NOT NULL,
	[S_2] [tinyint] NOT NULL,
	[B_2] [tinyint] NOT NULL,
	[Gg_2] [int] NOT NULL,
	[AB_2] [int] NOT NULL,
	[SB_2] [int] NOT NULL,
	[Sp0_2] [int] NOT NULL,
	[Sp1_2] [int] NOT NULL,
	[Sp2_2] [int] NOT NULL,
	[Sp3_2] [int] NOT NULL,
	[Sp4_2] [int] NOT NULL,
	[Sp5_2] [int] NOT NULL,
	[Sp6_2] [int] NOT NULL,
	[Sp7_2] [int] NOT NULL,
	[KF_2] [int] NOT NULL,
	[PF_2] [int] NOT NULL,
	[Cut_2] [bigint] NOT NULL,
	[F_3] [tinyint] NOT NULL,
	[H_3] [tinyint] NOT NULL,
	[U_3] [tinyint] NOT NULL,
	[L_3] [tinyint] NOT NULL,
	[A_3] [tinyint] NOT NULL,
	[S_3] [tinyint] NOT NULL,
	[B_3] [tinyint] NOT NULL,
	[Gg_3] [int] NOT NULL,
	[AB_3] [int] NOT NULL,
	[SB_3] [int] NOT NULL,
	[Sp0_3] [int] NOT NULL,
	[Sp1_3] [int] NOT NULL,
	[Sp2_3] [int] NOT NULL,
	[Sp3_3] [int] NOT NULL,
	[Sp4_3] [int] NOT NULL,
	[Sp5_3] [int] NOT NULL,
	[Sp6_3] [int] NOT NULL,
	[Sp7_3] [int] NOT NULL,
	[KF_3] [int] NOT NULL,
	[PF_3] [int] NOT NULL,
	[Cut_3] [bigint] NOT NULL,
 CONSTRAINT [PK_tbl_unit] PRIMARY KEY CLUSTERED 
(
	[Serial] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[tbl_unitdata]    Script Date: 12/01/2026 16:57:19 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[tbl_unitdata](
	[Serial] [int] NOT NULL,
	[Data0] [float] NOT NULL,
	[Data1] [float] NOT NULL,
 CONSTRAINT [PK_tbl_unitdata] PRIMARY KEY CLUSTERED 
(
	[Serial] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[tbl_unitlog]    Script Date: 12/01/2026 16:57:19 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[tbl_unitlog](
	[ID] [int] IDENTITY(1,1) NOT NULL,
	[Serial] [int] NOT NULL,
	[Name] [varchar](17) NOT NULL,
	[DID] [tinyint] NOT NULL,
	[OrgValue] [float] NOT NULL,
	[ChangeValue] [float] NOT NULL,
	[LogDate] [datetime] NOT NULL,
 CONSTRAINT [PK_tbl_unitlog] PRIMARY KEY CLUSTERED 
(
	[ID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[tbl_userinterface]    Script Date: 12/01/2026 16:57:19 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[tbl_userinterface](
	[Serial] [int] NOT NULL,
	[DCK] [bit] NOT NULL,
	[LB0] [smallint] NOT NULL,
	[LB1] [smallint] NOT NULL,
	[LB2] [smallint] NOT NULL,
	[LB3] [smallint] NOT NULL,
	[LB4] [smallint] NOT NULL,
	[LB5] [smallint] NOT NULL,
	[LB6] [smallint] NOT NULL,
	[LB7] [smallint] NOT NULL,
	[LB8] [smallint] NOT NULL,
	[LB9] [smallint] NOT NULL,
	[LB10] [smallint] NOT NULL,
	[LB11] [smallint] NOT NULL,
	[LB12] [smallint] NOT NULL,
	[LB13] [smallint] NOT NULL,
	[LB14] [smallint] NOT NULL,
	[LB15] [smallint] NOT NULL,
	[LB16] [smallint] NOT NULL,
	[LB17] [smallint] NOT NULL,
	[LB18] [smallint] NOT NULL,
	[LB19] [smallint] NOT NULL,
	[LB20] [smallint] NOT NULL,
	[LB21] [smallint] NOT NULL,
	[LB22] [smallint] NOT NULL,
	[LB23] [smallint] NOT NULL,
	[LB24] [smallint] NOT NULL,
	[LB25] [smallint] NOT NULL,
	[LB26] [smallint] NOT NULL,
	[LB27] [smallint] NOT NULL,
	[LB28] [smallint] NOT NULL,
	[LB29] [smallint] NOT NULL,
	[LB30] [smallint] NOT NULL,
	[LB31] [smallint] NOT NULL,
	[LB32] [smallint] NOT NULL,
	[LB33] [smallint] NOT NULL,
	[LB34] [smallint] NOT NULL,
	[LB35] [smallint] NOT NULL,
	[LB36] [smallint] NOT NULL,
	[LB37] [smallint] NOT NULL,
	[LB38] [smallint] NOT NULL,
	[LB39] [smallint] NOT NULL,
	[LB40] [smallint] NOT NULL,
	[LB41] [smallint] NOT NULL,
	[LB42] [smallint] NOT NULL,
	[LB43] [smallint] NOT NULL,
	[LB44] [smallint] NOT NULL,
	[LB45] [smallint] NOT NULL,
	[LB46] [smallint] NOT NULL,
	[LB47] [smallint] NOT NULL,
	[LB48] [smallint] NOT NULL,
	[LB49] [smallint] NOT NULL,
	[DCF0] [int] NOT NULL,
	[DCF1] [int] NOT NULL,
	[DCF2] [int] NOT NULL,
	[DCF3] [int] NOT NULL,
	[DCF4] [int] NOT NULL,
	[DCF5] [int] NOT NULL,
	[DCF6] [int] NOT NULL,
	[DCF7] [int] NOT NULL,
	[HCF0] [int] NOT NULL,
	[HCF1] [int] NOT NULL,
	[HCF2] [int] NOT NULL,
	[HCF3] [int] NOT NULL,
	[HCF4] [int] NOT NULL,
	[HCF5] [int] NOT NULL,
	[HCF6] [int] NOT NULL,
	[HCF7] [int] NOT NULL,
	[SkillWin0] [int] NOT NULL,
	[SKillWin1] [int] NOT NULL,
	[ForceWin0] [int] NOT NULL,
	[ForceWin1] [int] NOT NULL,
	[CharWin0] [int] NOT NULL,
	[CharWin1] [int] NOT NULL,
	[AnimusWin0] [int] NOT NULL,
	[AnimusWin1] [int] NOT NULL,
	[InvenWin] [int] NOT NULL,
	[InvenBag0] [int] NOT NULL,
	[InvenBag1] [int] NOT NULL,
	[InvenBag2] [int] NOT NULL,
	[InvenBag3] [int] NOT NULL,
	[InvenBag4] [int] NOT NULL,
	[LBLock] [tinyint] NOT NULL,
 CONSTRAINT [PK_tbl_userinterface] PRIMARY KEY CLUSTERED 
(
	[Serial] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[tbl_utgroupiteminfo]    Script Date: 12/01/2026 16:57:19 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[tbl_utgroupiteminfo](
	[serial] [int] IDENTITY(1,1) NOT NULL,
	[dck] [bit] NOT NULL,
	[item1] [int] NOT NULL,
	[item2] [int] NOT NULL,
	[item3] [int] NOT NULL,
	[item4] [int] NOT NULL,
	[item5] [int] NOT NULL,
	[item6] [int] NOT NULL,
 CONSTRAINT [PK_tbl_utgroupiteminfo_serial] PRIMARY KEY CLUSTERED 
(
	[serial] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[tbl_utresultinfo]    Script Date: 12/01/2026 16:57:19 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[tbl_utresultinfo](
	[type] [tinyint] NOT NULL,
	[serial] [int] NOT NULL,
	[state] [tinyint] NOT NULL,
	[resultdate] [datetime] NOT NULL,
	[buyer] [int] NOT NULL,
	[tax] [int] NOT NULL,
 CONSTRAINT [PK_tbl_utresultinfo_type_serial] PRIMARY KEY CLUSTERED 
(
	[type] ASC,
	[serial] ASC,
	[state] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[tbl_utresultstateid]    Script Date: 12/01/2026 16:57:19 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[tbl_utresultstateid](
	[id] [int] NOT NULL,
	[desc] [nvarchar](128) NOT NULL,
 CONSTRAINT [tbl_utresultstateid_id] PRIMARY KEY CLUSTERED 
(
	[id] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[tbl_utsellinfo]    Script Date: 12/01/2026 16:57:19 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[tbl_utsellinfo](
	[type] [tinyint] NOT NULL,
	[serial] [int] NOT NULL,
	[race] [tinyint] NOT NULL,
	[regdate] [datetime] NOT NULL,
	[owner] [int] NOT NULL,
	[price] [float] NOT NULL,
	[sellturm] [tinyint] NOT NULL,
 CONSTRAINT [PK_tbl_utsellinfo_type_serial] PRIMARY KEY CLUSTERED 
(
	[type] ASC,
	[serial] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[tbl_utsingleiteminfo]    Script Date: 12/01/2026 16:57:19 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[tbl_utsingleiteminfo](
	[serial] [int] IDENTITY(1,1) NOT NULL,
	[dck] [bit] NOT NULL,
	[inveninx] [tinyint] NOT NULL,
	[k] [int] NOT NULL,
	[d] [bigint] NOT NULL,
	[u] [int] NOT NULL,
	[lv] [tinyint] NOT NULL,
	[grade] [tinyint] NOT NULL,
	[class1] [tinyint] NOT NULL,
	[class2] [tinyint] NOT NULL,
	[class3] [tinyint] NOT NULL,
	[s] [bigint] NOT NULL,
	[t] [int] NOT NULL,
 CONSTRAINT [PK_tbl_utsingleiteminfo_serial] PRIMARY KEY CLUSTERED 
(
	[serial] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[tbl_WeeklyGuildPVPPointSum]    Script Date: 12/01/2026 16:57:19 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[tbl_WeeklyGuildPVPPointSum](
	[serial] [int] NOT NULL,
	[killpvppoint] [float] NOT NULL,
	[guildbattlepvppoint] [float] NOT NULL,
 CONSTRAINT [PK_tbl_WeeklyGuildPVPPointSum_serial] PRIMARY KEY CLUSTERED 
(
	[serial] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
SET ANSI_PADDING ON
GO
/****** Object:  Index [IX_tbl_characterselect_log_202410_account_logdate]    Script Date: 12/01/2026 16:57:19 ******/
CREATE NONCLUSTERED INDEX [IX_tbl_characterselect_log_202410_account_logdate] ON [dbo].[tbl_characterselect_log_202410]
(
	[Account] ASC,
	[LogDate] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, SORT_IN_TEMPDB = OFF, DROP_EXISTING = OFF, ONLINE = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
GO
SET ANSI_PADDING ON
GO
/****** Object:  Index [IX_tbl_characterselect_log_202410_characname_logdate]    Script Date: 12/01/2026 16:57:19 ******/
CREATE NONCLUSTERED INDEX [IX_tbl_characterselect_log_202410_characname_logdate] ON [dbo].[tbl_characterselect_log_202410]
(
	[CharacName] ASC,
	[LogDate] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, SORT_IN_TEMPDB = OFF, DROP_EXISTING = OFF, ONLINE = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
GO
SET ANSI_PADDING ON
GO
/****** Object:  Index [IX_tbl_characterselect_log_202601_account_logdate]    Script Date: 12/01/2026 16:57:19 ******/
CREATE NONCLUSTERED INDEX [IX_tbl_characterselect_log_202601_account_logdate] ON [dbo].[tbl_characterselect_log_202601]
(
	[Account] ASC,
	[LogDate] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, SORT_IN_TEMPDB = OFF, DROP_EXISTING = OFF, ONLINE = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
GO
SET ANSI_PADDING ON
GO
/****** Object:  Index [IX_tbl_characterselect_log_202601_characname_logdate]    Script Date: 12/01/2026 16:57:19 ******/
CREATE NONCLUSTERED INDEX [IX_tbl_characterselect_log_202601_characname_logdate] ON [dbo].[tbl_characterselect_log_202601]
(
	[CharacName] ASC,
	[LogDate] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, SORT_IN_TEMPDB = OFF, DROP_EXISTING = OFF, ONLINE = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
GO
ALTER TABLE [dbo].[deleteitem] ADD  CONSTRAINT [DF_deleteitem_itemname]  DEFAULT ('*') FOR [itemname]
GO
ALTER TABLE [dbo].[deleteitem] ADD  DEFAULT (0x0FFFFFFF) FOR [ucode]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_Accou__Accou__6F8A7843]  DEFAULT ((-1)) FOR [AccountSerial]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_Account__DCK__707E9C7C]  DEFAULT ((0)) FOR [DCK]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_Accou__EstSl__7172C0B5]  DEFAULT ((0)) FOR [EstSlot]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_Accou__Trunk__7266E4EE]  DEFAULT ((0)) FOR [TrunkPass]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_Accou__HintI__735B0927]  DEFAULT (0xFF) FOR [HintIndex]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_Accou__HintA__744F2D60]  DEFAULT ('*') FOR [HintAnswer]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_AccountT__K0__75435199]  DEFAULT (0xFFFFFFFF) FOR [K0]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF_tbl_accounttrunk_D0]  DEFAULT ((0)) FOR [D0]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_AccountT__U0__772B9A0B]  DEFAULT (0x0FFFFFFF) FOR [U0]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_AccountT__R0__781FBE44]  DEFAULT (0xFF) FOR [R0]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_AccountT__K1__7913E27D]  DEFAULT (0xFFFFFFFF) FOR [K1]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF_tbl_accounttrunk_D1]  DEFAULT ((0)) FOR [D1]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_AccountT__U1__7AFC2AEF]  DEFAULT (0x0FFFFFFF) FOR [U1]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_AccountT__R1__7BF04F28]  DEFAULT (0xFF) FOR [R1]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_AccountT__K2__7CE47361]  DEFAULT (0xFFFFFFFF) FOR [K2]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF_tbl_accounttrunk_D2]  DEFAULT ((0)) FOR [D2]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_AccountT__U2__7ECCBBD3]  DEFAULT (0x0FFFFFFF) FOR [U2]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_AccountT__R2__7FC0E00C]  DEFAULT (0xFF) FOR [R2]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_AccountT__K3__00B50445]  DEFAULT (0xFFFFFFFF) FOR [K3]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF_tbl_accounttrunk_D3]  DEFAULT ((0)) FOR [D3]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_AccountT__U3__029D4CB7]  DEFAULT (0x0FFFFFFF) FOR [U3]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_AccountT__R3__039170F0]  DEFAULT (0xFF) FOR [R3]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_AccountT__K4__04859529]  DEFAULT (0xFFFFFFFF) FOR [K4]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF_tbl_accounttrunk_D4]  DEFAULT ((0)) FOR [D4]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_AccountT__U4__066DDD9B]  DEFAULT (0x0FFFFFFF) FOR [U4]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_AccountT__R4__076201D4]  DEFAULT (0xFF) FOR [R4]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_AccountT__K5__0856260D]  DEFAULT (0xFFFFFFFF) FOR [K5]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF_tbl_accounttrunk_D5]  DEFAULT ((0)) FOR [D5]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_AccountT__U5__0A3E6E7F]  DEFAULT (0x0FFFFFFF) FOR [U5]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_AccountT__R5__0B3292B8]  DEFAULT (0xFF) FOR [R5]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_AccountT__K6__0C26B6F1]  DEFAULT (0xFFFFFFFF) FOR [K6]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF_tbl_accounttrunk_D6]  DEFAULT ((0)) FOR [D6]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_AccountT__U6__0E0EFF63]  DEFAULT (0x0FFFFFFF) FOR [U6]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_AccountT__R6__0F03239C]  DEFAULT (0xFF) FOR [R6]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_AccountT__K7__0FF747D5]  DEFAULT (0xFFFFFFFF) FOR [K7]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF_tbl_accounttrunk_D7]  DEFAULT ((0)) FOR [D7]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_AccountT__U7__11DF9047]  DEFAULT (0x0FFFFFFF) FOR [U7]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_AccountT__R7__12D3B480]  DEFAULT (0xFF) FOR [R7]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_AccountT__K8__13C7D8B9]  DEFAULT (0xFFFFFFFF) FOR [K8]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF_tbl_accounttrunk_D8]  DEFAULT ((0)) FOR [D8]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_AccountT__U8__15B0212B]  DEFAULT (0x0FFFFFFF) FOR [U8]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_AccountT__R8__16A44564]  DEFAULT (0xFF) FOR [R8]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_AccountT__K9__1798699D]  DEFAULT (0xFFFFFFFF) FOR [K9]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF_tbl_accounttrunk_D9]  DEFAULT ((0)) FOR [D9]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_AccountT__U9__1980B20F]  DEFAULT (0x0FFFFFFF) FOR [U9]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_AccountT__R9__1A74D648]  DEFAULT (0xFF) FOR [R9]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_Account__K10__1B68FA81]  DEFAULT (0xFFFFFFFF) FOR [K10]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF_tbl_accounttrunk_D10]  DEFAULT ((0)) FOR [D10]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_Account__U10__1D5142F3]  DEFAULT (0x0FFFFFFF) FOR [U10]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_Account__R10__1E45672C]  DEFAULT (0xFF) FOR [R10]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_Account__K11__1F398B65]  DEFAULT (0xFFFFFFFF) FOR [K11]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF_tbl_accounttrunk_D11]  DEFAULT ((0)) FOR [D11]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_Account__U11__2121D3D7]  DEFAULT (0x0FFFFFFF) FOR [U11]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_Account__R11__2215F810]  DEFAULT (0xFF) FOR [R11]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_Account__K12__230A1C49]  DEFAULT (0xFFFFFFFF) FOR [K12]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF_tbl_accounttrunk_D12]  DEFAULT ((0)) FOR [D12]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_Account__U12__24F264BB]  DEFAULT (0x0FFFFFFF) FOR [U12]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_Account__R12__25E688F4]  DEFAULT (0xFF) FOR [R12]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_Account__K13__26DAAD2D]  DEFAULT (0xFFFFFFFF) FOR [K13]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF_tbl_accounttrunk_D13]  DEFAULT ((0)) FOR [D13]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_Account__U13__28C2F59F]  DEFAULT (0x0FFFFFFF) FOR [U13]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_Account__R13__29B719D8]  DEFAULT (0xFF) FOR [R13]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_Account__K14__2AAB3E11]  DEFAULT (0xFFFFFFFF) FOR [K14]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF_tbl_accounttrunk_D14]  DEFAULT ((0)) FOR [D14]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_Account__U14__2C938683]  DEFAULT (0x0FFFFFFF) FOR [U14]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_Account__R14__2D87AABC]  DEFAULT (0xFF) FOR [R14]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_Account__K15__2E7BCEF5]  DEFAULT (0xFFFFFFFF) FOR [K15]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF_tbl_accounttrunk_D15]  DEFAULT ((0)) FOR [D15]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_Account__U15__30641767]  DEFAULT (0x0FFFFFFF) FOR [U15]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_Account__R15__31583BA0]  DEFAULT (0xFF) FOR [R15]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_Account__K16__324C5FD9]  DEFAULT (0xFFFFFFFF) FOR [K16]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF_tbl_accounttrunk_D16]  DEFAULT ((0)) FOR [D16]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_Account__U16__3434A84B]  DEFAULT (0x0FFFFFFF) FOR [U16]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_Account__R16__3528CC84]  DEFAULT (0xFF) FOR [R16]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_Account__K17__361CF0BD]  DEFAULT (0xFFFFFFFF) FOR [K17]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF_tbl_accounttrunk_D17]  DEFAULT ((0)) FOR [D17]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_Account__U17__3805392F]  DEFAULT (0x0FFFFFFF) FOR [U17]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_Account__R17__38F95D68]  DEFAULT (0xFF) FOR [R17]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_Account__K18__39ED81A1]  DEFAULT (0xFFFFFFFF) FOR [K18]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF_tbl_accounttrunk_D18]  DEFAULT ((0)) FOR [D18]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_Account__U18__3BD5CA13]  DEFAULT (0x0FFFFFFF) FOR [U18]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_Account__R18__3CC9EE4C]  DEFAULT (0xFF) FOR [R18]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_Account__K19__3DBE1285]  DEFAULT (0xFFFFFFFF) FOR [K19]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF_tbl_accounttrunk_D19]  DEFAULT ((0)) FOR [D19]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_Account__U19__3FA65AF7]  DEFAULT (0x0FFFFFFF) FOR [U19]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_Account__R19__409A7F30]  DEFAULT (0xFF) FOR [R19]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_Account__K20__418EA369]  DEFAULT (0xFFFFFFFF) FOR [K20]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF_tbl_accounttrunk_D20]  DEFAULT ((0)) FOR [D20]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_Account__U20__4376EBDB]  DEFAULT (0x0FFFFFFF) FOR [U20]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_Account__R20__446B1014]  DEFAULT (0xFF) FOR [R20]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_Account__K21__455F344D]  DEFAULT (0xFFFFFFFF) FOR [K21]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF_tbl_accounttrunk_D21]  DEFAULT ((0)) FOR [D21]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_Account__U21__47477CBF]  DEFAULT (0x0FFFFFFF) FOR [U21]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_Account__R21__483BA0F8]  DEFAULT (0xFF) FOR [R21]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_Account__K22__492FC531]  DEFAULT (0xFFFFFFFF) FOR [K22]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF_tbl_accounttrunk_D22]  DEFAULT ((0)) FOR [D22]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_Account__U22__4B180DA3]  DEFAULT (0x0FFFFFFF) FOR [U22]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_Account__R22__4C0C31DC]  DEFAULT (0xFF) FOR [R22]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_Account__K23__4D005615]  DEFAULT (0xFFFFFFFF) FOR [K23]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF_tbl_accounttrunk_D23]  DEFAULT ((0)) FOR [D23]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_Account__U23__4EE89E87]  DEFAULT (0x0FFFFFFF) FOR [U23]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_Account__R23__4FDCC2C0]  DEFAULT (0xFF) FOR [R23]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_Account__K24__50D0E6F9]  DEFAULT (0xFFFFFFFF) FOR [K24]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF_tbl_accounttrunk_D24]  DEFAULT ((0)) FOR [D24]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_Account__U24__52B92F6B]  DEFAULT (0x0FFFFFFF) FOR [U24]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_Account__R24__53AD53A4]  DEFAULT (0xFF) FOR [R24]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_Account__K25__54A177DD]  DEFAULT (0xFFFFFFFF) FOR [K25]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF_tbl_accounttrunk_D25]  DEFAULT ((0)) FOR [D25]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_Account__U25__5689C04F]  DEFAULT (0x0FFFFFFF) FOR [U25]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_Account__R25__577DE488]  DEFAULT (0xFF) FOR [R25]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_Account__K26__587208C1]  DEFAULT (0xFFFFFFFF) FOR [K26]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF_tbl_accounttrunk_D26]  DEFAULT ((0)) FOR [D26]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_Account__U26__5A5A5133]  DEFAULT (0x0FFFFFFF) FOR [U26]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_Account__R26__5B4E756C]  DEFAULT (0xFF) FOR [R26]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_Account__K27__5C4299A5]  DEFAULT (0xFFFFFFFF) FOR [K27]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF_tbl_accounttrunk_D27]  DEFAULT ((0)) FOR [D27]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_Account__U27__5E2AE217]  DEFAULT (0x0FFFFFFF) FOR [U27]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_Account__R27__5F1F0650]  DEFAULT (0xFF) FOR [R27]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_Account__K28__60132A89]  DEFAULT (0xFFFFFFFF) FOR [K28]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF_tbl_accounttrunk_D28]  DEFAULT ((0)) FOR [D28]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_Account__U28__61FB72FB]  DEFAULT (0x0FFFFFFF) FOR [U28]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_Account__R28__62EF9734]  DEFAULT (0xFF) FOR [R28]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_Account__K29__63E3BB6D]  DEFAULT (0xFFFFFFFF) FOR [K29]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF_tbl_accounttrunk_D29]  DEFAULT ((0)) FOR [D29]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_Account__U29__65CC03DF]  DEFAULT (0x0FFFFFFF) FOR [U29]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_Account__R29__66C02818]  DEFAULT (0xFF) FOR [R29]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_Account__K30__67B44C51]  DEFAULT (0xFFFFFFFF) FOR [K30]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF_tbl_accounttrunk_D30]  DEFAULT ((0)) FOR [D30]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_Account__U30__699C94C3]  DEFAULT (0x0FFFFFFF) FOR [U30]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_Account__R30__6A90B8FC]  DEFAULT (0xFF) FOR [R30]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_Account__K31__6B84DD35]  DEFAULT (0xFFFFFFFF) FOR [K31]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF_tbl_accounttrunk_D31]  DEFAULT ((0)) FOR [D31]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_Account__U31__6D6D25A7]  DEFAULT (0x0FFFFFFF) FOR [U31]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_Account__R31__6E6149E0]  DEFAULT (0xFF) FOR [R31]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_Account__K32__6F556E19]  DEFAULT (0xFFFFFFFF) FOR [K32]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF_tbl_accounttrunk_D32]  DEFAULT ((0)) FOR [D32]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_Account__U32__713DB68B]  DEFAULT (0x0FFFFFFF) FOR [U32]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_Account__R32__7231DAC4]  DEFAULT (0xFF) FOR [R32]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_Account__K33__7325FEFD]  DEFAULT (0xFFFFFFFF) FOR [K33]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF_tbl_accounttrunk_D33]  DEFAULT ((0)) FOR [D33]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_Account__U33__750E476F]  DEFAULT (0x0FFFFFFF) FOR [U33]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_Account__R33__76026BA8]  DEFAULT (0xFF) FOR [R33]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_Account__K34__76F68FE1]  DEFAULT (0xFFFFFFFF) FOR [K34]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF_tbl_accounttrunk_D34]  DEFAULT ((0)) FOR [D34]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_Account__U34__78DED853]  DEFAULT (0x0FFFFFFF) FOR [U34]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_Account__R34__79D2FC8C]  DEFAULT (0xFF) FOR [R34]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_Account__K35__7AC720C5]  DEFAULT (0xFFFFFFFF) FOR [K35]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF_tbl_accounttrunk_D35]  DEFAULT ((0)) FOR [D35]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_Account__U35__7CAF6937]  DEFAULT (0x0FFFFFFF) FOR [U35]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_Account__R35__7DA38D70]  DEFAULT (0xFF) FOR [R35]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_Account__K36__7E97B1A9]  DEFAULT (0xFFFFFFFF) FOR [K36]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF_tbl_accounttrunk_D36]  DEFAULT ((0)) FOR [D36]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_Account__U36__007FFA1B]  DEFAULT (0x0FFFFFFF) FOR [U36]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_Account__R36__01741E54]  DEFAULT (0xFF) FOR [R36]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_Account__K37__0268428D]  DEFAULT (0xFFFFFFFF) FOR [K37]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF_tbl_accounttrunk_D37]  DEFAULT ((0)) FOR [D37]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_Account__U37__04508AFF]  DEFAULT (0x0FFFFFFF) FOR [U37]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_Account__R37__0544AF38]  DEFAULT (0xFF) FOR [R37]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_Account__K38__0638D371]  DEFAULT (0xFFFFFFFF) FOR [K38]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF_tbl_accounttrunk_D38]  DEFAULT ((0)) FOR [D38]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_Account__U38__08211BE3]  DEFAULT (0x0FFFFFFF) FOR [U38]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_Account__R38__0915401C]  DEFAULT (0xFF) FOR [R38]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_Account__K39__0A096455]  DEFAULT (0xFFFFFFFF) FOR [K39]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF_tbl_accounttrunk_D39]  DEFAULT ((0)) FOR [D39]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_Account__U39__0BF1ACC7]  DEFAULT (0x0FFFFFFF) FOR [U39]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_Account__R39__0CE5D100]  DEFAULT (0xFF) FOR [R39]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_Account__K40__0DD9F539]  DEFAULT (0xFFFFFFFF) FOR [K40]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF_tbl_accounttrunk_D40]  DEFAULT ((0)) FOR [D40]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_Account__U40__0FC23DAB]  DEFAULT (0x0FFFFFFF) FOR [U40]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_Account__R40__10B661E4]  DEFAULT (0xFF) FOR [R40]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_Account__K41__11AA861D]  DEFAULT (0xFFFFFFFF) FOR [K41]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF_tbl_accounttrunk_D41]  DEFAULT ((0)) FOR [D41]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_Account__U41__1392CE8F]  DEFAULT (0x0FFFFFFF) FOR [U41]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_Account__R41__1486F2C8]  DEFAULT (0xFF) FOR [R41]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_Account__K42__157B1701]  DEFAULT (0xFFFFFFFF) FOR [K42]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF_tbl_accounttrunk_D42]  DEFAULT ((0)) FOR [D42]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_Account__U42__17635F73]  DEFAULT (0x0FFFFFFF) FOR [U42]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_Account__R42__185783AC]  DEFAULT (0xFF) FOR [R42]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_Account__K43__194BA7E5]  DEFAULT (0xFFFFFFFF) FOR [K43]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF_tbl_accounttrunk_D43]  DEFAULT ((0)) FOR [D43]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_Account__U43__1B33F057]  DEFAULT (0x0FFFFFFF) FOR [U43]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_Account__R43__1C281490]  DEFAULT (0xFF) FOR [R43]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_Account__K44__1D1C38C9]  DEFAULT (0xFFFFFFFF) FOR [K44]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF_tbl_accounttrunk_D44]  DEFAULT ((0)) FOR [D44]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_Account__U44__1F04813B]  DEFAULT (0x0FFFFFFF) FOR [U44]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_Account__R44__1FF8A574]  DEFAULT (0xFF) FOR [R44]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_Account__K45__20ECC9AD]  DEFAULT (0xFFFFFFFF) FOR [K45]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF_tbl_accounttrunk_D45]  DEFAULT ((0)) FOR [D45]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_Account__U45__22D5121F]  DEFAULT (0x0FFFFFFF) FOR [U45]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_Account__R45__23C93658]  DEFAULT (0xFF) FOR [R45]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_Account__K46__24BD5A91]  DEFAULT (0xFFFFFFFF) FOR [K46]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF_tbl_accounttrunk_D46]  DEFAULT ((0)) FOR [D46]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_Account__U46__26A5A303]  DEFAULT (0x0FFFFFFF) FOR [U46]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_Account__R46__2799C73C]  DEFAULT (0xFF) FOR [R46]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_Account__K47__288DEB75]  DEFAULT (0xFFFFFFFF) FOR [K47]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF_tbl_accounttrunk_D47]  DEFAULT ((0)) FOR [D47]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_Account__U47__2A7633E7]  DEFAULT (0x0FFFFFFF) FOR [U47]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_Account__R47__2B6A5820]  DEFAULT (0xFF) FOR [R47]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_Account__K48__2C5E7C59]  DEFAULT (0xFFFFFFFF) FOR [K48]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF_tbl_accounttrunk_D48]  DEFAULT ((0)) FOR [D48]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_Account__U48__2E46C4CB]  DEFAULT (0x0FFFFFFF) FOR [U48]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_Account__R48__2F3AE904]  DEFAULT (0xFF) FOR [R48]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_Account__K49__302F0D3D]  DEFAULT (0xFFFFFFFF) FOR [K49]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF_tbl_accounttrunk_D49]  DEFAULT ((0)) FOR [D49]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_Account__U49__321755AF]  DEFAULT (0x0FFFFFFF) FOR [U49]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_Account__R49__330B79E8]  DEFAULT (0xFF) FOR [R49]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_Account__K50__33FF9E21]  DEFAULT (0xFFFFFFFF) FOR [K50]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF_tbl_accounttrunk_D50]  DEFAULT ((0)) FOR [D50]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_Account__U50__35E7E693]  DEFAULT (0x0FFFFFFF) FOR [U50]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_Account__R50__36DC0ACC]  DEFAULT (0xFF) FOR [R50]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_Account__K51__37D02F05]  DEFAULT (0xFFFFFFFF) FOR [K51]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF_tbl_accounttrunk_D51]  DEFAULT ((0)) FOR [D51]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_Account__U51__39B87777]  DEFAULT (0x0FFFFFFF) FOR [U51]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_Account__R51__3AAC9BB0]  DEFAULT (0xFF) FOR [R51]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_Account__K52__3BA0BFE9]  DEFAULT (0xFFFFFFFF) FOR [K52]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF_tbl_accounttrunk_D52]  DEFAULT ((0)) FOR [D52]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_Account__U52__3D89085B]  DEFAULT (0x0FFFFFFF) FOR [U52]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_Account__R52__3E7D2C94]  DEFAULT (0xFF) FOR [R52]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_Account__K53__3F7150CD]  DEFAULT (0xFFFFFFFF) FOR [K53]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF_tbl_accounttrunk_D53]  DEFAULT ((0)) FOR [D53]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_Account__U53__4159993F]  DEFAULT (0x0FFFFFFF) FOR [U53]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_Account__R53__424DBD78]  DEFAULT (0xFF) FOR [R53]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_Account__K54__4341E1B1]  DEFAULT (0xFFFFFFFF) FOR [K54]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF_tbl_accounttrunk_D54]  DEFAULT ((0)) FOR [D54]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_Account__U54__452A2A23]  DEFAULT (0x0FFFFFFF) FOR [U54]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_Account__R54__461E4E5C]  DEFAULT (0xFF) FOR [R54]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_Account__K55__47127295]  DEFAULT (0xFFFFFFFF) FOR [K55]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF_tbl_accounttrunk_D55]  DEFAULT ((0)) FOR [D55]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_Account__U55__48FABB07]  DEFAULT (0x0FFFFFFF) FOR [U55]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_Account__R55__49EEDF40]  DEFAULT (0xFF) FOR [R55]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_Account__K56__4AE30379]  DEFAULT (0xFFFFFFFF) FOR [K56]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF_tbl_accounttrunk_D56]  DEFAULT ((0)) FOR [D56]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_Account__U56__4CCB4BEB]  DEFAULT (0x0FFFFFFF) FOR [U56]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_Account__R56__4DBF7024]  DEFAULT (0xFF) FOR [R56]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_Account__K57__4EB3945D]  DEFAULT (0xFFFFFFFF) FOR [K57]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF_tbl_accounttrunk_D57]  DEFAULT ((0)) FOR [D57]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_Account__U57__509BDCCF]  DEFAULT (0x0FFFFFFF) FOR [U57]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_Account__R57__51900108]  DEFAULT (0xFF) FOR [R57]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_Account__K58__52842541]  DEFAULT (0xFFFFFFFF) FOR [K58]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF_tbl_accounttrunk_D58]  DEFAULT ((0)) FOR [D58]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_Account__U58__546C6DB3]  DEFAULT (0x0FFFFFFF) FOR [U58]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_Account__R58__556091EC]  DEFAULT (0xFF) FOR [R58]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_Account__K59__5654B625]  DEFAULT (0xFFFFFFFF) FOR [K59]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF_tbl_accounttrunk_D59]  DEFAULT ((0)) FOR [D59]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_Account__U59__583CFE97]  DEFAULT (0x0FFFFFFF) FOR [U59]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_Account__R59__593122D0]  DEFAULT (0xFF) FOR [R59]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_Account__K60__5A254709]  DEFAULT (0xFFFFFFFF) FOR [K60]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF_tbl_accounttrunk_D60]  DEFAULT ((0)) FOR [D60]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_Account__U60__5C0D8F7B]  DEFAULT (0x0FFFFFFF) FOR [U60]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_Account__R60__5D01B3B4]  DEFAULT (0xFF) FOR [R60]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_Account__K61__5DF5D7ED]  DEFAULT (0xFFFFFFFF) FOR [K61]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF_tbl_accounttrunk_D61]  DEFAULT ((0)) FOR [D61]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_Account__U61__5FDE205F]  DEFAULT (0x0FFFFFFF) FOR [U61]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_Account__R61__60D24498]  DEFAULT (0xFF) FOR [R61]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_Account__K62__61C668D1]  DEFAULT (0xFFFFFFFF) FOR [K62]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF_tbl_accounttrunk_D62]  DEFAULT ((0)) FOR [D62]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_Account__U62__63AEB143]  DEFAULT (0x0FFFFFFF) FOR [U62]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_Account__R62__64A2D57C]  DEFAULT (0xFF) FOR [R62]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_Account__K63__6596F9B5]  DEFAULT (0xFFFFFFFF) FOR [K63]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF_tbl_accounttrunk_D63]  DEFAULT ((0)) FOR [D63]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_Account__U63__677F4227]  DEFAULT (0x0FFFFFFF) FOR [U63]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_Account__R63__68736660]  DEFAULT (0xFF) FOR [R63]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_Account__K64__69678A99]  DEFAULT (0xFFFFFFFF) FOR [K64]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF_tbl_accounttrunk_D64]  DEFAULT ((0)) FOR [D64]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_Account__U64__6B4FD30B]  DEFAULT (0x0FFFFFFF) FOR [U64]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_Account__R64__6C43F744]  DEFAULT (0xFF) FOR [R64]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_Account__K65__6D381B7D]  DEFAULT (0xFFFFFFFF) FOR [K65]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF_tbl_accounttrunk_D65]  DEFAULT ((0)) FOR [D65]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_Account__U65__6F2063EF]  DEFAULT (0x0FFFFFFF) FOR [U65]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_Account__R65__70148828]  DEFAULT (0xFF) FOR [R65]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_Account__K66__7108AC61]  DEFAULT (0xFFFFFFFF) FOR [K66]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF_tbl_accounttrunk_D66]  DEFAULT ((0)) FOR [D66]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_Account__U66__72F0F4D3]  DEFAULT (0x0FFFFFFF) FOR [U66]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_Account__R66__73E5190C]  DEFAULT (0xFF) FOR [R66]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_Account__K67__74D93D45]  DEFAULT (0xFFFFFFFF) FOR [K67]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF_tbl_accounttrunk_D67]  DEFAULT ((0)) FOR [D67]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_Account__U67__76C185B7]  DEFAULT (0x0FFFFFFF) FOR [U67]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_Account__R67__77B5A9F0]  DEFAULT (0xFF) FOR [R67]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_Account__K68__78A9CE29]  DEFAULT (0xFFFFFFFF) FOR [K68]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF_tbl_accounttrunk_D68]  DEFAULT ((0)) FOR [D68]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_Account__U68__7A92169B]  DEFAULT (0x0FFFFFFF) FOR [U68]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_Account__R68__7B863AD4]  DEFAULT (0xFF) FOR [R68]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_Account__K69__7C7A5F0D]  DEFAULT (0xFFFFFFFF) FOR [K69]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF_tbl_accounttrunk_D69]  DEFAULT ((0)) FOR [D69]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_Account__U69__7E62A77F]  DEFAULT (0x0FFFFFFF) FOR [U69]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_Account__R69__7F56CBB8]  DEFAULT (0xFF) FOR [R69]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_Account__K70__004AEFF1]  DEFAULT (0xFFFFFFFF) FOR [K70]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF_tbl_accounttrunk_D70]  DEFAULT ((0)) FOR [D70]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_Account__U70__02333863]  DEFAULT (0x0FFFFFFF) FOR [U70]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_Account__R70__03275C9C]  DEFAULT (0xFF) FOR [R70]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_Account__K71__041B80D5]  DEFAULT (0xFFFFFFFF) FOR [K71]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF_tbl_accounttrunk_D71]  DEFAULT ((0)) FOR [D71]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_Account__U71__0603C947]  DEFAULT (0x0FFFFFFF) FOR [U71]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_Account__R71__06F7ED80]  DEFAULT (0xFF) FOR [R71]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_Account__K72__07EC11B9]  DEFAULT (0xFFFFFFFF) FOR [K72]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF_tbl_accounttrunk_D72]  DEFAULT ((0)) FOR [D72]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_Account__U72__09D45A2B]  DEFAULT (0x0FFFFFFF) FOR [U72]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_Account__R72__0AC87E64]  DEFAULT (0xFF) FOR [R72]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_Account__K73__0BBCA29D]  DEFAULT (0xFFFFFFFF) FOR [K73]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF_tbl_accounttrunk_D73]  DEFAULT ((0)) FOR [D73]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_Account__U73__0DA4EB0F]  DEFAULT (0x0FFFFFFF) FOR [U73]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_Account__R73__0E990F48]  DEFAULT (0xFF) FOR [R73]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_Account__K74__0F8D3381]  DEFAULT (0xFFFFFFFF) FOR [K74]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF_tbl_accounttrunk_D74]  DEFAULT ((0)) FOR [D74]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_Account__U74__11757BF3]  DEFAULT (0x0FFFFFFF) FOR [U74]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_Account__R74__1269A02C]  DEFAULT (0xFF) FOR [R74]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_Account__K75__135DC465]  DEFAULT (0xFFFFFFFF) FOR [K75]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF_tbl_accounttrunk_D75]  DEFAULT ((0)) FOR [D75]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_Account__U75__15460CD7]  DEFAULT (0x0FFFFFFF) FOR [U75]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_Account__R75__163A3110]  DEFAULT (0xFF) FOR [R75]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_Account__K76__172E5549]  DEFAULT (0xFFFFFFFF) FOR [K76]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF_tbl_accounttrunk_D76]  DEFAULT ((0)) FOR [D76]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_Account__U76__19169DBB]  DEFAULT (0x0FFFFFFF) FOR [U76]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_Account__R76__1A0AC1F4]  DEFAULT (0xFF) FOR [R76]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_Account__K77__1AFEE62D]  DEFAULT (0xFFFFFFFF) FOR [K77]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF_tbl_accounttrunk_D77]  DEFAULT ((0)) FOR [D77]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_Account__U77__1CE72E9F]  DEFAULT (0x0FFFFFFF) FOR [U77]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_Account__R77__1DDB52D8]  DEFAULT (0xFF) FOR [R77]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_Account__K78__1ECF7711]  DEFAULT (0xFFFFFFFF) FOR [K78]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF_tbl_accounttrunk_D78]  DEFAULT ((0)) FOR [D78]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_Account__U78__20B7BF83]  DEFAULT (0x0FFFFFFF) FOR [U78]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_Account__R78__21ABE3BC]  DEFAULT (0xFF) FOR [R78]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_Account__K79__22A007F5]  DEFAULT (0xFFFFFFFF) FOR [K79]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF_tbl_accounttrunk_D79]  DEFAULT ((0)) FOR [D79]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_Account__U79__24885067]  DEFAULT (0x0FFFFFFF) FOR [U79]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_Account__R79__257C74A0]  DEFAULT (0xFF) FOR [R79]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_Account__K80__267098D9]  DEFAULT (0xFFFFFFFF) FOR [K80]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF_tbl_accounttrunk_D80]  DEFAULT ((0)) FOR [D80]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_Account__U80__2858E14B]  DEFAULT (0x0FFFFFFF) FOR [U80]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_Account__R80__294D0584]  DEFAULT (0xFF) FOR [R80]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_Account__K81__2A4129BD]  DEFAULT (0xFFFFFFFF) FOR [K81]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF_tbl_accounttrunk_D81]  DEFAULT ((0)) FOR [D81]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_Account__U81__2C29722F]  DEFAULT (0x0FFFFFFF) FOR [U81]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_Account__R81__2D1D9668]  DEFAULT (0xFF) FOR [R81]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_Account__K82__2E11BAA1]  DEFAULT (0xFFFFFFFF) FOR [K82]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF_tbl_accounttrunk_D82]  DEFAULT ((0)) FOR [D82]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_Account__U82__2FFA0313]  DEFAULT (0x0FFFFFFF) FOR [U82]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_Account__R82__30EE274C]  DEFAULT (0xFF) FOR [R82]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_Account__K83__31E24B85]  DEFAULT (0xFFFFFFFF) FOR [K83]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF_tbl_accounttrunk_D83]  DEFAULT ((0)) FOR [D83]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_Account__U83__33CA93F7]  DEFAULT (0x0FFFFFFF) FOR [U83]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_Account__R83__34BEB830]  DEFAULT (0xFF) FOR [R83]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_Account__K84__35B2DC69]  DEFAULT (0xFFFFFFFF) FOR [K84]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF_tbl_accounttrunk_D84]  DEFAULT ((0)) FOR [D84]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_Account__U84__379B24DB]  DEFAULT (0x0FFFFFFF) FOR [U84]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_Account__R84__388F4914]  DEFAULT (0xFF) FOR [R84]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_Account__K85__39836D4D]  DEFAULT (0xFFFFFFFF) FOR [K85]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF_tbl_accounttrunk_D85]  DEFAULT ((0)) FOR [D85]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_Account__U85__3B6BB5BF]  DEFAULT (0x0FFFFFFF) FOR [U85]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_Account__R85__3C5FD9F8]  DEFAULT (0xFF) FOR [R85]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_Account__K86__3D53FE31]  DEFAULT (0xFFFFFFFF) FOR [K86]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF_tbl_accounttrunk_D86]  DEFAULT ((0)) FOR [D86]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_Account__U86__3F3C46A3]  DEFAULT (0x0FFFFFFF) FOR [U86]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_Account__R86__40306ADC]  DEFAULT (0xFF) FOR [R86]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_Account__K87__41248F15]  DEFAULT (0xFFFFFFFF) FOR [K87]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF_tbl_accounttrunk_D87]  DEFAULT ((0)) FOR [D87]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_Account__U87__430CD787]  DEFAULT (0x0FFFFFFF) FOR [U87]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_Account__R87__4400FBC0]  DEFAULT (0xFF) FOR [R87]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_Account__K88__44F51FF9]  DEFAULT (0xFFFFFFFF) FOR [K88]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF_tbl_accounttrunk_D88]  DEFAULT ((0)) FOR [D88]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_Account__U88__46DD686B]  DEFAULT (0x0FFFFFFF) FOR [U88]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_Account__R88__47D18CA4]  DEFAULT (0xFF) FOR [R88]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_Account__K89__48C5B0DD]  DEFAULT (0xFFFFFFFF) FOR [K89]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF_tbl_accounttrunk_D89]  DEFAULT ((0)) FOR [D89]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_Account__U89__4AADF94F]  DEFAULT (0x0FFFFFFF) FOR [U89]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_Account__R89__4BA21D88]  DEFAULT (0xFF) FOR [R89]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_Account__K90__4C9641C1]  DEFAULT (0xFFFFFFFF) FOR [K90]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF_tbl_accounttrunk_D90]  DEFAULT ((0)) FOR [D90]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_Account__U90__4E7E8A33]  DEFAULT (0x0FFFFFFF) FOR [U90]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_Account__R90__4F72AE6C]  DEFAULT (0xFF) FOR [R90]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_Account__K91__5066D2A5]  DEFAULT (0xFFFFFFFF) FOR [K91]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF_tbl_accounttrunk_D91]  DEFAULT ((0)) FOR [D91]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_Account__U91__524F1B17]  DEFAULT (0x0FFFFFFF) FOR [U91]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_Account__R91__53433F50]  DEFAULT (0xFF) FOR [R91]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_Account__K92__54376389]  DEFAULT (0xFFFFFFFF) FOR [K92]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF_tbl_accounttrunk_D92]  DEFAULT ((0)) FOR [D92]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_Account__U92__561FABFB]  DEFAULT (0x0FFFFFFF) FOR [U92]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_Account__R92__5713D034]  DEFAULT (0xFF) FOR [R92]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_Account__K93__5807F46D]  DEFAULT (0xFFFFFFFF) FOR [K93]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF_tbl_accounttrunk_D93]  DEFAULT ((0)) FOR [D93]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_Account__U93__59F03CDF]  DEFAULT (0x0FFFFFFF) FOR [U93]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_Account__R93__5AE46118]  DEFAULT (0xFF) FOR [R93]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_Account__K94__5BD88551]  DEFAULT (0xFFFFFFFF) FOR [K94]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF_tbl_accounttrunk_D94]  DEFAULT ((0)) FOR [D94]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_Account__U94__5DC0CDC3]  DEFAULT (0x0FFFFFFF) FOR [U94]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_Account__R94__5EB4F1FC]  DEFAULT (0xFF) FOR [R94]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_Account__K95__5FA91635]  DEFAULT (0xFFFFFFFF) FOR [K95]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF_tbl_accounttrunk_D95]  DEFAULT ((0)) FOR [D95]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_Account__U95__61915EA7]  DEFAULT (0x0FFFFFFF) FOR [U95]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_Account__R95__628582E0]  DEFAULT (0xFF) FOR [R95]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_Account__K96__6379A719]  DEFAULT (0xFFFFFFFF) FOR [K96]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF_tbl_accounttrunk_D96]  DEFAULT ((0)) FOR [D96]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_Account__U96__6561EF8B]  DEFAULT (0x0FFFFFFF) FOR [U96]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_Account__R96__665613C4]  DEFAULT (0xFF) FOR [R96]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_Account__K97__674A37FD]  DEFAULT (0xFFFFFFFF) FOR [K97]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF_tbl_accounttrunk_D97]  DEFAULT ((0)) FOR [D97]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_Account__U97__6932806F]  DEFAULT (0x0FFFFFFF) FOR [U97]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_Account__R97__6A26A4A8]  DEFAULT (0xFF) FOR [R97]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_Account__K98__6B1AC8E1]  DEFAULT (0xFFFFFFFF) FOR [K98]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF_tbl_accounttrunk_D98]  DEFAULT ((0)) FOR [D98]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_Account__U98__6D031153]  DEFAULT (0x0FFFFFFF) FOR [U98]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_Account__R98__6DF7358C]  DEFAULT (0xFF) FOR [R98]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_Account__K99__6EEB59C5]  DEFAULT (0xFFFFFFFF) FOR [K99]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF_tbl_accounttrunk_D99]  DEFAULT ((0)) FOR [D99]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_Account__U99__70D3A237]  DEFAULT (0x0FFFFFFF) FOR [U99]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_Account__R99__71C7C670]  DEFAULT (0xFF) FOR [R99]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_Accou__BDala__72BBEAA9]  DEFAULT ((0)) FOR [Dalant0]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_Accou__BGold__73B00EE2]  DEFAULT ((0)) FOR [Gold0]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_Accou__CDala__74A4331B]  DEFAULT ((0)) FOR [Dalant1]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_Accou__CGold__75985754]  DEFAULT ((0)) FOR [Gold1]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_Accou__ADala__768C7B8D]  DEFAULT ((0)) FOR [Dalant2]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_Accou__AGold__77809FC6]  DEFAULT ((0)) FOR [Gold2]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_accountt__S0__3585C3A5]  DEFAULT ((0)) FOR [S0]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_accountt__T0__3679E7DE]  DEFAULT (0xFFFFFFFF) FOR [T0]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_accountt__S1__376E0C17]  DEFAULT ((0)) FOR [S1]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_accountt__T1__38623050]  DEFAULT (0xFFFFFFFF) FOR [T1]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_accountt__S2__39565489]  DEFAULT ((0)) FOR [S2]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_accountt__T2__3A4A78C2]  DEFAULT (0xFFFFFFFF) FOR [T2]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_accountt__S3__3B3E9CFB]  DEFAULT ((0)) FOR [S3]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_accountt__T3__3C32C134]  DEFAULT (0xFFFFFFFF) FOR [T3]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_accountt__S4__3D26E56D]  DEFAULT ((0)) FOR [S4]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_accountt__T4__3E1B09A6]  DEFAULT (0xFFFFFFFF) FOR [T4]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_accountt__S5__3F0F2DDF]  DEFAULT ((0)) FOR [S5]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_accountt__T5__40035218]  DEFAULT (0xFFFFFFFF) FOR [T5]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_accountt__S6__40F77651]  DEFAULT ((0)) FOR [S6]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_accountt__T6__41EB9A8A]  DEFAULT (0xFFFFFFFF) FOR [T6]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_accountt__S7__42DFBEC3]  DEFAULT ((0)) FOR [S7]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_accountt__T7__43D3E2FC]  DEFAULT (0xFFFFFFFF) FOR [T7]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_accountt__S8__44C80735]  DEFAULT ((0)) FOR [S8]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_accountt__T8__45BC2B6E]  DEFAULT (0xFFFFFFFF) FOR [T8]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_accountt__S9__46B04FA7]  DEFAULT ((0)) FOR [S9]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_accountt__T9__47A473E0]  DEFAULT (0xFFFFFFFF) FOR [T9]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_account__S10__48989819]  DEFAULT ((0)) FOR [S10]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_account__T10__498CBC52]  DEFAULT (0xFFFFFFFF) FOR [T10]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_account__S11__4A80E08B]  DEFAULT ((0)) FOR [S11]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_account__T11__4B7504C4]  DEFAULT (0xFFFFFFFF) FOR [T11]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_account__S12__4C6928FD]  DEFAULT ((0)) FOR [S12]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_account__T12__4D5D4D36]  DEFAULT (0xFFFFFFFF) FOR [T12]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_account__S13__4E51716F]  DEFAULT ((0)) FOR [S13]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_account__T13__4F4595A8]  DEFAULT (0xFFFFFFFF) FOR [T13]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_account__S14__5039B9E1]  DEFAULT ((0)) FOR [S14]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_account__T14__512DDE1A]  DEFAULT (0xFFFFFFFF) FOR [T14]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_account__S15__52220253]  DEFAULT ((0)) FOR [S15]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_account__T15__5316268C]  DEFAULT (0xFFFFFFFF) FOR [T15]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_account__S16__540A4AC5]  DEFAULT ((0)) FOR [S16]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_account__T16__54FE6EFE]  DEFAULT (0xFFFFFFFF) FOR [T16]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_account__S17__55F29337]  DEFAULT ((0)) FOR [S17]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_account__T17__56E6B770]  DEFAULT (0xFFFFFFFF) FOR [T17]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_account__S18__57DADBA9]  DEFAULT ((0)) FOR [S18]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_account__T18__58CEFFE2]  DEFAULT (0xFFFFFFFF) FOR [T18]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_account__S19__59C3241B]  DEFAULT ((0)) FOR [S19]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_account__T19__5AB74854]  DEFAULT (0xFFFFFFFF) FOR [T19]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_account__S20__5BAB6C8D]  DEFAULT ((0)) FOR [S20]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_account__T20__5C9F90C6]  DEFAULT (0xFFFFFFFF) FOR [T20]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_account__S21__5D93B4FF]  DEFAULT ((0)) FOR [S21]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_account__T21__5E87D938]  DEFAULT (0xFFFFFFFF) FOR [T21]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_account__S22__5F7BFD71]  DEFAULT ((0)) FOR [S22]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_account__T22__607021AA]  DEFAULT (0xFFFFFFFF) FOR [T22]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_account__S23__616445E3]  DEFAULT ((0)) FOR [S23]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_account__T23__62586A1C]  DEFAULT (0xFFFFFFFF) FOR [T23]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_account__S24__634C8E55]  DEFAULT ((0)) FOR [S24]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_account__T24__6440B28E]  DEFAULT (0xFFFFFFFF) FOR [T24]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_account__S25__6534D6C7]  DEFAULT ((0)) FOR [S25]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_account__T25__6628FB00]  DEFAULT (0xFFFFFFFF) FOR [T25]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_account__S26__671D1F39]  DEFAULT ((0)) FOR [S26]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_account__T26__68114372]  DEFAULT (0xFFFFFFFF) FOR [T26]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_account__S27__690567AB]  DEFAULT ((0)) FOR [S27]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_account__T27__69F98BE4]  DEFAULT (0xFFFFFFFF) FOR [T27]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_account__S28__6AEDB01D]  DEFAULT ((0)) FOR [S28]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_account__T28__6BE1D456]  DEFAULT (0xFFFFFFFF) FOR [T28]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_account__S29__6CD5F88F]  DEFAULT ((0)) FOR [S29]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_account__T29__6DCA1CC8]  DEFAULT (0xFFFFFFFF) FOR [T29]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_account__S30__6EBE4101]  DEFAULT ((0)) FOR [S30]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_account__T30__6FB2653A]  DEFAULT (0xFFFFFFFF) FOR [T30]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_account__S31__70A68973]  DEFAULT ((0)) FOR [S31]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_account__T31__719AADAC]  DEFAULT (0xFFFFFFFF) FOR [T31]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_account__S32__728ED1E5]  DEFAULT ((0)) FOR [S32]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_account__T32__7382F61E]  DEFAULT (0xFFFFFFFF) FOR [T32]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_account__S33__74771A57]  DEFAULT ((0)) FOR [S33]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_account__T33__756B3E90]  DEFAULT (0xFFFFFFFF) FOR [T33]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_account__S34__765F62C9]  DEFAULT ((0)) FOR [S34]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_account__T34__77538702]  DEFAULT (0xFFFFFFFF) FOR [T34]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_account__S35__7847AB3B]  DEFAULT ((0)) FOR [S35]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_account__T35__793BCF74]  DEFAULT (0xFFFFFFFF) FOR [T35]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_account__S36__7A2FF3AD]  DEFAULT ((0)) FOR [S36]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_account__T36__7B2417E6]  DEFAULT (0xFFFFFFFF) FOR [T36]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_account__S37__7C183C1F]  DEFAULT ((0)) FOR [S37]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_account__T37__7D0C6058]  DEFAULT (0xFFFFFFFF) FOR [T37]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_account__S38__7E008491]  DEFAULT ((0)) FOR [S38]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_account__T38__7EF4A8CA]  DEFAULT (0xFFFFFFFF) FOR [T38]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_account__S39__7FE8CD03]  DEFAULT ((0)) FOR [S39]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_account__T39__00DCF13C]  DEFAULT (0xFFFFFFFF) FOR [T39]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_account__S40__01D11575]  DEFAULT ((0)) FOR [S40]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_account__T40__02C539AE]  DEFAULT (0xFFFFFFFF) FOR [T40]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_account__S41__03B95DE7]  DEFAULT ((0)) FOR [S41]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_account__T41__04AD8220]  DEFAULT (0xFFFFFFFF) FOR [T41]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_account__S42__05A1A659]  DEFAULT ((0)) FOR [S42]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_account__T42__0695CA92]  DEFAULT (0xFFFFFFFF) FOR [T42]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_account__S43__0789EECB]  DEFAULT ((0)) FOR [S43]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_account__T43__087E1304]  DEFAULT (0xFFFFFFFF) FOR [T43]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_account__S44__0972373D]  DEFAULT ((0)) FOR [S44]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_account__T44__0A665B76]  DEFAULT (0xFFFFFFFF) FOR [T44]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_account__S45__0B5A7FAF]  DEFAULT ((0)) FOR [S45]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_account__T45__0C4EA3E8]  DEFAULT (0xFFFFFFFF) FOR [T45]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_account__S46__0D42C821]  DEFAULT ((0)) FOR [S46]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_account__T46__0E36EC5A]  DEFAULT (0xFFFFFFFF) FOR [T46]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_account__S47__0F2B1093]  DEFAULT ((0)) FOR [S47]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_account__T47__101F34CC]  DEFAULT (0xFFFFFFFF) FOR [T47]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_account__S48__11135905]  DEFAULT ((0)) FOR [S48]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_account__T48__12077D3E]  DEFAULT (0xFFFFFFFF) FOR [T48]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_account__S49__12FBA177]  DEFAULT ((0)) FOR [S49]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_account__T49__13EFC5B0]  DEFAULT (0xFFFFFFFF) FOR [T49]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_account__S50__14E3E9E9]  DEFAULT ((0)) FOR [S50]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_account__T50__15D80E22]  DEFAULT (0xFFFFFFFF) FOR [T50]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_account__S51__16CC325B]  DEFAULT ((0)) FOR [S51]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_account__T51__17C05694]  DEFAULT (0xFFFFFFFF) FOR [T51]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_account__S52__18B47ACD]  DEFAULT ((0)) FOR [S52]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_account__T52__19A89F06]  DEFAULT (0xFFFFFFFF) FOR [T52]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_account__S53__1A9CC33F]  DEFAULT ((0)) FOR [S53]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_account__T53__1B90E778]  DEFAULT (0xFFFFFFFF) FOR [T53]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_account__S54__1C850BB1]  DEFAULT ((0)) FOR [S54]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_account__T54__1D792FEA]  DEFAULT (0xFFFFFFFF) FOR [T54]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_account__S55__1E6D5423]  DEFAULT ((0)) FOR [S55]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_account__T55__1F61785C]  DEFAULT (0xFFFFFFFF) FOR [T55]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_account__S56__20559C95]  DEFAULT ((0)) FOR [S56]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_account__T56__2149C0CE]  DEFAULT (0xFFFFFFFF) FOR [T56]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_account__S57__223DE507]  DEFAULT ((0)) FOR [S57]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_account__T57__23320940]  DEFAULT (0xFFFFFFFF) FOR [T57]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_account__S58__24262D79]  DEFAULT ((0)) FOR [S58]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_account__T58__251A51B2]  DEFAULT (0xFFFFFFFF) FOR [T58]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_account__S59__260E75EB]  DEFAULT ((0)) FOR [S59]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_account__T59__27029A24]  DEFAULT (0xFFFFFFFF) FOR [T59]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_account__S60__27F6BE5D]  DEFAULT ((0)) FOR [S60]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_account__T60__28EAE296]  DEFAULT (0xFFFFFFFF) FOR [T60]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_account__S61__29DF06CF]  DEFAULT ((0)) FOR [S61]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_account__T61__2AD32B08]  DEFAULT (0xFFFFFFFF) FOR [T61]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_account__S62__2BC74F41]  DEFAULT ((0)) FOR [S62]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_account__T62__2CBB737A]  DEFAULT (0xFFFFFFFF) FOR [T62]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_account__S63__2DAF97B3]  DEFAULT ((0)) FOR [S63]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_account__T63__2EA3BBEC]  DEFAULT (0xFFFFFFFF) FOR [T63]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_account__S64__2F97E025]  DEFAULT ((0)) FOR [S64]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_account__T64__308C045E]  DEFAULT (0xFFFFFFFF) FOR [T64]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_account__S65__31802897]  DEFAULT ((0)) FOR [S65]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_account__T65__32744CD0]  DEFAULT (0xFFFFFFFF) FOR [T65]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_account__S66__33687109]  DEFAULT ((0)) FOR [S66]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_account__T66__345C9542]  DEFAULT (0xFFFFFFFF) FOR [T66]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_account__S67__3550B97B]  DEFAULT ((0)) FOR [S67]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_account__T67__3644DDB4]  DEFAULT (0xFFFFFFFF) FOR [T67]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_account__S68__373901ED]  DEFAULT ((0)) FOR [S68]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_account__T68__382D2626]  DEFAULT (0xFFFFFFFF) FOR [T68]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_account__S69__39214A5F]  DEFAULT ((0)) FOR [S69]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_account__T69__3A156E98]  DEFAULT (0xFFFFFFFF) FOR [T69]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_account__S70__3B0992D1]  DEFAULT ((0)) FOR [S70]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_account__T70__3BFDB70A]  DEFAULT (0xFFFFFFFF) FOR [T70]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_account__S71__3CF1DB43]  DEFAULT ((0)) FOR [S71]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_account__T71__3DE5FF7C]  DEFAULT (0xFFFFFFFF) FOR [T71]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_account__S72__3EDA23B5]  DEFAULT ((0)) FOR [S72]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_account__T72__3FCE47EE]  DEFAULT (0xFFFFFFFF) FOR [T72]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_account__S73__40C26C27]  DEFAULT ((0)) FOR [S73]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_account__T73__41B69060]  DEFAULT (0xFFFFFFFF) FOR [T73]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_account__S74__42AAB499]  DEFAULT ((0)) FOR [S74]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_account__T74__439ED8D2]  DEFAULT (0xFFFFFFFF) FOR [T74]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_account__S75__4492FD0B]  DEFAULT ((0)) FOR [S75]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_account__T75__45872144]  DEFAULT (0xFFFFFFFF) FOR [T75]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_account__S76__467B457D]  DEFAULT ((0)) FOR [S76]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_account__T76__476F69B6]  DEFAULT (0xFFFFFFFF) FOR [T76]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_account__S77__48638DEF]  DEFAULT ((0)) FOR [S77]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_account__T77__4957B228]  DEFAULT (0xFFFFFFFF) FOR [T77]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_account__S78__4A4BD661]  DEFAULT ((0)) FOR [S78]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_account__T78__4B3FFA9A]  DEFAULT (0xFFFFFFFF) FOR [T78]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_account__S79__4C341ED3]  DEFAULT ((0)) FOR [S79]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_account__T79__4D28430C]  DEFAULT (0xFFFFFFFF) FOR [T79]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_account__S80__4E1C6745]  DEFAULT ((0)) FOR [S80]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_account__T80__4F108B7E]  DEFAULT (0xFFFFFFFF) FOR [T80]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_account__S81__5004AFB7]  DEFAULT ((0)) FOR [S81]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_account__T81__50F8D3F0]  DEFAULT (0xFFFFFFFF) FOR [T81]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_account__S82__51ECF829]  DEFAULT ((0)) FOR [S82]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_account__T82__52E11C62]  DEFAULT (0xFFFFFFFF) FOR [T82]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_account__S83__53D5409B]  DEFAULT ((0)) FOR [S83]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_account__T83__54C964D4]  DEFAULT (0xFFFFFFFF) FOR [T83]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_account__S84__55BD890D]  DEFAULT ((0)) FOR [S84]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_account__T84__56B1AD46]  DEFAULT (0xFFFFFFFF) FOR [T84]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_account__S85__57A5D17F]  DEFAULT ((0)) FOR [S85]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_account__T85__5899F5B8]  DEFAULT (0xFFFFFFFF) FOR [T85]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_account__S86__598E19F1]  DEFAULT ((0)) FOR [S86]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_account__T86__5A823E2A]  DEFAULT (0xFFFFFFFF) FOR [T86]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_account__S87__5B766263]  DEFAULT ((0)) FOR [S87]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_account__T87__5C6A869C]  DEFAULT (0xFFFFFFFF) FOR [T87]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_account__S88__5D5EAAD5]  DEFAULT ((0)) FOR [S88]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_account__T88__5E52CF0E]  DEFAULT (0xFFFFFFFF) FOR [T88]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_account__S89__5F46F347]  DEFAULT ((0)) FOR [S89]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_account__T89__603B1780]  DEFAULT (0xFFFFFFFF) FOR [T89]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_account__S90__612F3BB9]  DEFAULT ((0)) FOR [S90]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_account__T90__62235FF2]  DEFAULT (0xFFFFFFFF) FOR [T90]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_account__S91__6317842B]  DEFAULT ((0)) FOR [S91]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_account__T91__640BA864]  DEFAULT (0xFFFFFFFF) FOR [T91]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_account__S92__64FFCC9D]  DEFAULT ((0)) FOR [S92]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_account__T92__65F3F0D6]  DEFAULT (0xFFFFFFFF) FOR [T92]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_account__S93__66E8150F]  DEFAULT ((0)) FOR [S93]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_account__T93__67DC3948]  DEFAULT (0xFFFFFFFF) FOR [T93]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_account__S94__68D05D81]  DEFAULT ((0)) FOR [S94]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_account__T94__69C481BA]  DEFAULT (0xFFFFFFFF) FOR [T94]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_account__S95__6AB8A5F3]  DEFAULT ((0)) FOR [S95]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_account__T95__6BACCA2C]  DEFAULT (0xFFFFFFFF) FOR [T95]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_account__S96__6CA0EE65]  DEFAULT ((0)) FOR [S96]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_account__T96__6D95129E]  DEFAULT (0xFFFFFFFF) FOR [T96]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_account__S97__6E8936D7]  DEFAULT ((0)) FOR [S97]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_account__T97__6F7D5B10]  DEFAULT (0xFFFFFFFF) FOR [T97]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_account__S98__70717F49]  DEFAULT ((0)) FOR [S98]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_account__T98__7165A382]  DEFAULT (0xFFFFFFFF) FOR [T98]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_account__S99__7259C7BB]  DEFAULT ((0)) FOR [S99]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk] ADD  CONSTRAINT [DF__tbl_account__T99__734DEBF4]  DEFAULT (0xFFFFFFFF) FOR [T99]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk_Extend] ADD  CONSTRAINT [DF_tbl_AccountTrunk_Extend_AccountSerial]  DEFAULT ((-1)) FOR [AccountSerial]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk_Extend] ADD  CONSTRAINT [DF_tbl_AccountTrunk_Extend_DCK]  DEFAULT ((0)) FOR [DCK]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk_Extend] ADD  CONSTRAINT [DF_tbl_AccountTrunk_Extend_EstSlot]  DEFAULT ((0)) FOR [EstSlot]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk_Extend] ADD  CONSTRAINT [DF_tbl_AccountTrunk_Extend_K0]  DEFAULT (0xFFFFFFFF) FOR [K0]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk_Extend] ADD  CONSTRAINT [DF_tbl_AccountTrunk_Extend_D0]  DEFAULT ((0)) FOR [D0]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk_Extend] ADD  CONSTRAINT [DF_tbl_AccountTrunk_Extend_U0]  DEFAULT (0x0FFFFFFF) FOR [U0]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk_Extend] ADD  CONSTRAINT [DF_tbl_AccountTrunk_Extend_R0]  DEFAULT (0xFF) FOR [R0]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk_Extend] ADD  CONSTRAINT [DF_tbl_AccountTrunk_Extend_S0]  DEFAULT ((0)) FOR [S0]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk_Extend] ADD  CONSTRAINT [DF_tbl_AccountTrunk_Extend_T0]  DEFAULT (0xFFFFFFFF) FOR [T0]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk_Extend] ADD  CONSTRAINT [DF_tbl_AccountTrunk_Extend_K1]  DEFAULT (0xFFFFFFFF) FOR [K1]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk_Extend] ADD  CONSTRAINT [DF_tbl_AccountTrunk_Extend_D1]  DEFAULT ((0)) FOR [D1]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk_Extend] ADD  CONSTRAINT [DF_tbl_AccountTrunk_Extend_U1]  DEFAULT (0x0FFFFFFF) FOR [U1]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk_Extend] ADD  CONSTRAINT [DF_tbl_AccountTrunk_Extend_R1]  DEFAULT (0xFF) FOR [R1]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk_Extend] ADD  CONSTRAINT [DF_tbl_AccountTrunk_Extend_S1]  DEFAULT ((0)) FOR [S1]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk_Extend] ADD  CONSTRAINT [DF_tbl_AccountTrunk_Extend_T1]  DEFAULT (0xFFFFFFFF) FOR [T1]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk_Extend] ADD  CONSTRAINT [DF_tbl_AccountTrunk_Extend_K2]  DEFAULT (0xFFFFFFFF) FOR [K2]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk_Extend] ADD  CONSTRAINT [DF_tbl_AccountTrunk_Extend_D2]  DEFAULT ((0)) FOR [D2]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk_Extend] ADD  CONSTRAINT [DF_tbl_AccountTrunk_Extend_U2]  DEFAULT (0x0FFFFFFF) FOR [U2]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk_Extend] ADD  CONSTRAINT [DF_tbl_AccountTrunk_Extend_R2]  DEFAULT (0xFF) FOR [R2]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk_Extend] ADD  CONSTRAINT [DF_tbl_AccountTrunk_Extend_S2]  DEFAULT ((0)) FOR [S2]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk_Extend] ADD  CONSTRAINT [DF_tbl_AccountTrunk_Extend_T2]  DEFAULT (0xFFFFFFFF) FOR [T2]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk_Extend] ADD  CONSTRAINT [DF_tbl_AccountTrunk_Extend_K3]  DEFAULT (0xFFFFFFFF) FOR [K3]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk_Extend] ADD  CONSTRAINT [DF_tbl_AccountTrunk_Extend_D3]  DEFAULT ((0)) FOR [D3]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk_Extend] ADD  CONSTRAINT [DF_tbl_AccountTrunk_Extend_U3]  DEFAULT (0x0FFFFFFF) FOR [U3]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk_Extend] ADD  CONSTRAINT [DF_tbl_AccountTrunk_Extend_R3]  DEFAULT (0xFF) FOR [R3]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk_Extend] ADD  CONSTRAINT [DF_tbl_AccountTrunk_Extend_S3]  DEFAULT ((0)) FOR [S3]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk_Extend] ADD  CONSTRAINT [DF_tbl_AccountTrunk_Extend_T3]  DEFAULT (0xFFFFFFFF) FOR [T3]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk_Extend] ADD  CONSTRAINT [DF_tbl_AccountTrunk_Extend_K4]  DEFAULT (0xFFFFFFFF) FOR [K4]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk_Extend] ADD  CONSTRAINT [DF_tbl_AccountTrunk_Extend_D4]  DEFAULT ((0)) FOR [D4]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk_Extend] ADD  CONSTRAINT [DF_tbl_AccountTrunk_Extend_U4]  DEFAULT (0x0FFFFFFF) FOR [U4]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk_Extend] ADD  CONSTRAINT [DF_tbl_AccountTrunk_Extend_R4]  DEFAULT (0xFF) FOR [R4]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk_Extend] ADD  CONSTRAINT [DF_tbl_AccountTrunk_Extend_S4]  DEFAULT ((0)) FOR [S4]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk_Extend] ADD  CONSTRAINT [DF_tbl_AccountTrunk_Extend_T4]  DEFAULT (0xFFFFFFFF) FOR [T4]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk_Extend] ADD  CONSTRAINT [DF_tbl_AccountTrunk_Extend_K5]  DEFAULT (0xFFFFFFFF) FOR [K5]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk_Extend] ADD  CONSTRAINT [DF_tbl_AccountTrunk_Extend_D5]  DEFAULT ((0)) FOR [D5]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk_Extend] ADD  CONSTRAINT [DF_tbl_AccountTrunk_Extend_U5]  DEFAULT (0x0FFFFFFF) FOR [U5]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk_Extend] ADD  CONSTRAINT [DF_tbl_AccountTrunk_Extend_R5]  DEFAULT (0xFF) FOR [R5]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk_Extend] ADD  CONSTRAINT [DF_tbl_AccountTrunk_Extend_S5]  DEFAULT ((0)) FOR [S5]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk_Extend] ADD  CONSTRAINT [DF_tbl_AccountTrunk_Extend_T5]  DEFAULT (0xFFFFFFFF) FOR [T5]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk_Extend] ADD  CONSTRAINT [DF_tbl_AccountTrunk_Extend_K6]  DEFAULT (0xFFFFFFFF) FOR [K6]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk_Extend] ADD  CONSTRAINT [DF_tbl_AccountTrunk_Extend_D6]  DEFAULT ((0)) FOR [D6]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk_Extend] ADD  CONSTRAINT [DF_tbl_AccountTrunk_Extend_U6]  DEFAULT (0x0FFFFFFF) FOR [U6]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk_Extend] ADD  CONSTRAINT [DF_tbl_AccountTrunk_Extend_R6]  DEFAULT (0xFF) FOR [R6]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk_Extend] ADD  CONSTRAINT [DF_tbl_AccountTrunk_Extend_S6]  DEFAULT ((0)) FOR [S6]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk_Extend] ADD  CONSTRAINT [DF_tbl_AccountTrunk_Extend_T6]  DEFAULT (0xFFFFFFFF) FOR [T6]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk_Extend] ADD  CONSTRAINT [DF_tbl_AccountTrunk_Extend_K7]  DEFAULT (0xFFFFFFFF) FOR [K7]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk_Extend] ADD  CONSTRAINT [DF_tbl_AccountTrunk_Extend_D7]  DEFAULT ((0)) FOR [D7]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk_Extend] ADD  CONSTRAINT [DF_tbl_AccountTrunk_Extend_U7]  DEFAULT (0x0FFFFFFF) FOR [U7]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk_Extend] ADD  CONSTRAINT [DF_tbl_AccountTrunk_Extend_R7]  DEFAULT (0xFF) FOR [R7]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk_Extend] ADD  CONSTRAINT [DF_tbl_AccountTrunk_Extend_S7]  DEFAULT ((0)) FOR [S7]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk_Extend] ADD  CONSTRAINT [DF_tbl_AccountTrunk_Extend_T7]  DEFAULT (0xFFFFFFFF) FOR [T7]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk_Extend] ADD  CONSTRAINT [DF_tbl_AccountTrunk_Extend_K8]  DEFAULT (0xFFFFFFFF) FOR [K8]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk_Extend] ADD  CONSTRAINT [DF_tbl_AccountTrunk_Extend_D8]  DEFAULT ((0)) FOR [D8]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk_Extend] ADD  CONSTRAINT [DF_tbl_AccountTrunk_Extend_U8]  DEFAULT (0x0FFFFFFF) FOR [U8]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk_Extend] ADD  CONSTRAINT [DF_tbl_AccountTrunk_Extend_R8]  DEFAULT (0xFF) FOR [R8]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk_Extend] ADD  CONSTRAINT [DF_tbl_AccountTrunk_Extend_S8]  DEFAULT ((0)) FOR [S8]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk_Extend] ADD  CONSTRAINT [DF_tbl_AccountTrunk_Extend_T8]  DEFAULT (0xFFFFFFFF) FOR [T8]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk_Extend] ADD  CONSTRAINT [DF_tbl_AccountTrunk_Extend_K9]  DEFAULT (0xFFFFFFFF) FOR [K9]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk_Extend] ADD  CONSTRAINT [DF_tbl_AccountTrunk_Extend_D9]  DEFAULT ((0)) FOR [D9]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk_Extend] ADD  CONSTRAINT [DF_tbl_AccountTrunk_Extend_U9]  DEFAULT (0x0FFFFFFF) FOR [U9]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk_Extend] ADD  CONSTRAINT [DF_tbl_AccountTrunk_Extend_R9]  DEFAULT (0xFF) FOR [R9]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk_Extend] ADD  CONSTRAINT [DF_tbl_AccountTrunk_Extend_S9]  DEFAULT ((0)) FOR [S9]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk_Extend] ADD  CONSTRAINT [DF_tbl_AccountTrunk_Extend_T9]  DEFAULT (0xFFFFFFFF) FOR [T9]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk_Extend] ADD  CONSTRAINT [DF_tbl_AccountTrunk_Extend_K10]  DEFAULT (0xFFFFFFFF) FOR [K10]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk_Extend] ADD  CONSTRAINT [DF_tbl_AccountTrunk_Extend_D10]  DEFAULT ((0)) FOR [D10]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk_Extend] ADD  CONSTRAINT [DF_tbl_AccountTrunk_Extend_U10]  DEFAULT (0x0FFFFFFF) FOR [U10]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk_Extend] ADD  CONSTRAINT [DF_tbl_AccountTrunk_Extend_R10]  DEFAULT (0xFF) FOR [R10]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk_Extend] ADD  CONSTRAINT [DF_tbl_AccountTrunk_Extend_S10]  DEFAULT ((0)) FOR [S10]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk_Extend] ADD  CONSTRAINT [DF_tbl_AccountTrunk_Extend_T10]  DEFAULT (0xFFFFFFFF) FOR [T10]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk_Extend] ADD  CONSTRAINT [DF_tbl_AccountTrunk_Extend_K11]  DEFAULT (0xFFFFFFFF) FOR [K11]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk_Extend] ADD  CONSTRAINT [DF_tbl_AccountTrunk_Extend_D11]  DEFAULT ((0)) FOR [D11]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk_Extend] ADD  CONSTRAINT [DF_tbl_AccountTrunk_Extend_U11]  DEFAULT (0x0FFFFFFF) FOR [U11]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk_Extend] ADD  CONSTRAINT [DF_tbl_AccountTrunk_Extend_R11]  DEFAULT (0xFF) FOR [R11]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk_Extend] ADD  CONSTRAINT [DF_tbl_AccountTrunk_Extend_S11]  DEFAULT ((0)) FOR [S11]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk_Extend] ADD  CONSTRAINT [DF_tbl_AccountTrunk_Extend_T11]  DEFAULT (0xFFFFFFFF) FOR [T11]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk_Extend] ADD  CONSTRAINT [DF_tbl_AccountTrunk_Extend_K12]  DEFAULT (0xFFFFFFFF) FOR [K12]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk_Extend] ADD  CONSTRAINT [DF_tbl_AccountTrunk_Extend_D12]  DEFAULT ((0)) FOR [D12]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk_Extend] ADD  CONSTRAINT [DF_tbl_AccountTrunk_Extend_U12]  DEFAULT (0x0FFFFFFF) FOR [U12]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk_Extend] ADD  CONSTRAINT [DF_tbl_AccountTrunk_Extend_R12]  DEFAULT (0xFF) FOR [R12]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk_Extend] ADD  CONSTRAINT [DF_tbl_AccountTrunk_Extend_S12]  DEFAULT ((0)) FOR [S12]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk_Extend] ADD  CONSTRAINT [DF_tbl_AccountTrunk_Extend_T12]  DEFAULT (0xFFFFFFFF) FOR [T12]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk_Extend] ADD  CONSTRAINT [DF_tbl_AccountTrunk_Extend_K13]  DEFAULT (0xFFFFFFFF) FOR [K13]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk_Extend] ADD  CONSTRAINT [DF_tbl_AccountTrunk_Extend_D13]  DEFAULT ((0)) FOR [D13]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk_Extend] ADD  CONSTRAINT [DF_tbl_AccountTrunk_Extend_U13]  DEFAULT (0x0FFFFFFF) FOR [U13]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk_Extend] ADD  CONSTRAINT [DF_tbl_AccountTrunk_Extend_R13]  DEFAULT (0xFF) FOR [R13]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk_Extend] ADD  CONSTRAINT [DF_tbl_AccountTrunk_Extend_S13]  DEFAULT ((0)) FOR [S13]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk_Extend] ADD  CONSTRAINT [DF_tbl_AccountTrunk_Extend_T13]  DEFAULT (0xFFFFFFFF) FOR [T13]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk_Extend] ADD  CONSTRAINT [DF_tbl_AccountTrunk_Extend_K14]  DEFAULT (0xFFFFFFFF) FOR [K14]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk_Extend] ADD  CONSTRAINT [DF_tbl_AccountTrunk_Extend_D14]  DEFAULT ((0)) FOR [D14]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk_Extend] ADD  CONSTRAINT [DF_tbl_AccountTrunk_Extend_U14]  DEFAULT (0x0FFFFFFF) FOR [U14]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk_Extend] ADD  CONSTRAINT [DF_tbl_AccountTrunk_Extend_R14]  DEFAULT (0xFF) FOR [R14]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk_Extend] ADD  CONSTRAINT [DF_tbl_AccountTrunk_Extend_S14]  DEFAULT ((0)) FOR [S14]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk_Extend] ADD  CONSTRAINT [DF_tbl_AccountTrunk_Extend_T14]  DEFAULT (0xFFFFFFFF) FOR [T14]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk_Extend] ADD  CONSTRAINT [DF_tbl_AccountTrunk_Extend_K15]  DEFAULT (0xFFFFFFFF) FOR [K15]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk_Extend] ADD  CONSTRAINT [DF_tbl_AccountTrunk_Extend_D15]  DEFAULT ((0)) FOR [D15]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk_Extend] ADD  CONSTRAINT [DF_tbl_AccountTrunk_Extend_U15]  DEFAULT (0x0FFFFFFF) FOR [U15]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk_Extend] ADD  CONSTRAINT [DF_tbl_AccountTrunk_Extend_R15]  DEFAULT (0xFF) FOR [R15]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk_Extend] ADD  CONSTRAINT [DF_tbl_AccountTrunk_Extend_S15]  DEFAULT ((0)) FOR [S15]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk_Extend] ADD  CONSTRAINT [DF_tbl_AccountTrunk_Extend_T15]  DEFAULT (0xFFFFFFFF) FOR [T15]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk_Extend] ADD  CONSTRAINT [DF_tbl_AccountTrunk_Extend_K16]  DEFAULT (0xFFFFFFFF) FOR [K16]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk_Extend] ADD  CONSTRAINT [DF_tbl_AccountTrunk_Extend_D16]  DEFAULT ((0)) FOR [D16]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk_Extend] ADD  CONSTRAINT [DF_tbl_AccountTrunk_Extend_U16]  DEFAULT (0x0FFFFFFF) FOR [U16]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk_Extend] ADD  CONSTRAINT [DF_tbl_AccountTrunk_Extend_R16]  DEFAULT (0xFF) FOR [R16]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk_Extend] ADD  CONSTRAINT [DF_tbl_AccountTrunk_Extend_S16]  DEFAULT ((0)) FOR [S16]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk_Extend] ADD  CONSTRAINT [DF_tbl_AccountTrunk_Extend_T16]  DEFAULT (0xFFFFFFFF) FOR [T16]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk_Extend] ADD  CONSTRAINT [DF_tbl_AccountTrunk_Extend_K17]  DEFAULT (0xFFFFFFFF) FOR [K17]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk_Extend] ADD  CONSTRAINT [DF_tbl_AccountTrunk_Extend_D17]  DEFAULT ((0)) FOR [D17]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk_Extend] ADD  CONSTRAINT [DF_tbl_AccountTrunk_Extend_U17]  DEFAULT (0x0FFFFFFF) FOR [U17]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk_Extend] ADD  CONSTRAINT [DF_tbl_AccountTrunk_Extend_R17]  DEFAULT (0xFF) FOR [R17]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk_Extend] ADD  CONSTRAINT [DF_tbl_AccountTrunk_Extend_S17]  DEFAULT ((0)) FOR [S17]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk_Extend] ADD  CONSTRAINT [DF_tbl_AccountTrunk_Extend_T17]  DEFAULT (0xFFFFFFFF) FOR [T17]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk_Extend] ADD  CONSTRAINT [DF_tbl_AccountTrunk_Extend_K18]  DEFAULT (0xFFFFFFFF) FOR [K18]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk_Extend] ADD  CONSTRAINT [DF_tbl_AccountTrunk_Extend_D18]  DEFAULT ((0)) FOR [D18]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk_Extend] ADD  CONSTRAINT [DF_tbl_AccountTrunk_Extend_U18]  DEFAULT (0x0FFFFFFF) FOR [U18]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk_Extend] ADD  CONSTRAINT [DF_tbl_AccountTrunk_Extend_R18]  DEFAULT (0xFF) FOR [R18]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk_Extend] ADD  CONSTRAINT [DF_tbl_AccountTrunk_Extend_S18]  DEFAULT ((0)) FOR [S18]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk_Extend] ADD  CONSTRAINT [DF_tbl_AccountTrunk_Extend_T18]  DEFAULT (0xFFFFFFFF) FOR [T18]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk_Extend] ADD  CONSTRAINT [DF_tbl_AccountTrunk_Extend_K19]  DEFAULT (0xFFFFFFFF) FOR [K19]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk_Extend] ADD  CONSTRAINT [DF_tbl_AccountTrunk_Extend_D19]  DEFAULT ((0)) FOR [D19]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk_Extend] ADD  CONSTRAINT [DF_tbl_AccountTrunk_Extend_U19]  DEFAULT (0x0FFFFFFF) FOR [U19]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk_Extend] ADD  CONSTRAINT [DF_tbl_AccountTrunk_Extend_R19]  DEFAULT (0xFF) FOR [R19]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk_Extend] ADD  CONSTRAINT [DF_tbl_AccountTrunk_Extend_S19]  DEFAULT ((0)) FOR [S19]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk_Extend] ADD  CONSTRAINT [DF_tbl_AccountTrunk_Extend_T19]  DEFAULT (0xFFFFFFFF) FOR [T19]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk_Extend] ADD  CONSTRAINT [DF_tbl_AccountTrunk_Extend_K20]  DEFAULT (0xFFFFFFFF) FOR [K20]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk_Extend] ADD  CONSTRAINT [DF_tbl_AccountTrunk_Extend_D20]  DEFAULT ((0)) FOR [D20]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk_Extend] ADD  CONSTRAINT [DF_tbl_AccountTrunk_Extend_U20]  DEFAULT (0x0FFFFFFF) FOR [U20]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk_Extend] ADD  CONSTRAINT [DF_tbl_AccountTrunk_Extend_R20]  DEFAULT (0xFF) FOR [R20]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk_Extend] ADD  CONSTRAINT [DF_tbl_AccountTrunk_Extend_S20]  DEFAULT ((0)) FOR [S20]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk_Extend] ADD  CONSTRAINT [DF_tbl_AccountTrunk_Extend_T20]  DEFAULT (0xFFFFFFFF) FOR [T20]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk_Extend] ADD  CONSTRAINT [DF_tbl_AccountTrunk_Extend_K21]  DEFAULT (0xFFFFFFFF) FOR [K21]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk_Extend] ADD  CONSTRAINT [DF_tbl_AccountTrunk_Extend_D21]  DEFAULT ((0)) FOR [D21]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk_Extend] ADD  CONSTRAINT [DF_tbl_AccountTrunk_Extend_U21]  DEFAULT (0x0FFFFFFF) FOR [U21]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk_Extend] ADD  CONSTRAINT [DF_tbl_AccountTrunk_Extend_R21]  DEFAULT (0xFF) FOR [R21]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk_Extend] ADD  CONSTRAINT [DF_tbl_AccountTrunk_Extend_S21]  DEFAULT ((0)) FOR [S21]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk_Extend] ADD  CONSTRAINT [DF_tbl_AccountTrunk_Extend_T21]  DEFAULT (0xFFFFFFFF) FOR [T21]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk_Extend] ADD  CONSTRAINT [DF_tbl_AccountTrunk_Extend_K22]  DEFAULT (0xFFFFFFFF) FOR [K22]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk_Extend] ADD  CONSTRAINT [DF_tbl_AccountTrunk_Extend_D22]  DEFAULT ((0)) FOR [D22]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk_Extend] ADD  CONSTRAINT [DF_tbl_AccountTrunk_Extend_U22]  DEFAULT (0x0FFFFFFF) FOR [U22]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk_Extend] ADD  CONSTRAINT [DF_tbl_AccountTrunk_Extend_R22]  DEFAULT (0xFF) FOR [R22]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk_Extend] ADD  CONSTRAINT [DF_tbl_AccountTrunk_Extend_S22]  DEFAULT ((0)) FOR [S22]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk_Extend] ADD  CONSTRAINT [DF_tbl_AccountTrunk_Extend_T22]  DEFAULT (0xFFFFFFFF) FOR [T22]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk_Extend] ADD  CONSTRAINT [DF_tbl_AccountTrunk_Extend_K23]  DEFAULT (0xFFFFFFFF) FOR [K23]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk_Extend] ADD  CONSTRAINT [DF_tbl_AccountTrunk_Extend_D23]  DEFAULT ((0)) FOR [D23]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk_Extend] ADD  CONSTRAINT [DF_tbl_AccountTrunk_Extend_U23]  DEFAULT (0x0FFFFFFF) FOR [U23]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk_Extend] ADD  CONSTRAINT [DF_tbl_AccountTrunk_Extend_R23]  DEFAULT (0xFF) FOR [R23]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk_Extend] ADD  CONSTRAINT [DF_tbl_AccountTrunk_Extend_S23]  DEFAULT ((0)) FOR [S23]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk_Extend] ADD  CONSTRAINT [DF_tbl_AccountTrunk_Extend_T23]  DEFAULT (0xFFFFFFFF) FOR [T23]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk_Extend] ADD  CONSTRAINT [DF_tbl_AccountTrunk_Extend_K24]  DEFAULT (0xFFFFFFFF) FOR [K24]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk_Extend] ADD  CONSTRAINT [DF_tbl_AccountTrunk_Extend_D24]  DEFAULT ((0)) FOR [D24]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk_Extend] ADD  CONSTRAINT [DF_tbl_AccountTrunk_Extend_U24]  DEFAULT (0x0FFFFFFF) FOR [U24]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk_Extend] ADD  CONSTRAINT [DF_tbl_AccountTrunk_Extend_R24]  DEFAULT (0xFF) FOR [R24]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk_Extend] ADD  CONSTRAINT [DF_tbl_AccountTrunk_Extend_S24]  DEFAULT ((0)) FOR [S24]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk_Extend] ADD  CONSTRAINT [DF_tbl_AccountTrunk_Extend_T24]  DEFAULT (0xFFFFFFFF) FOR [T24]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk_Extend] ADD  CONSTRAINT [DF_tbl_AccountTrunk_Extend_K25]  DEFAULT (0xFFFFFFFF) FOR [K25]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk_Extend] ADD  CONSTRAINT [DF_tbl_AccountTrunk_Extend_D25]  DEFAULT ((0)) FOR [D25]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk_Extend] ADD  CONSTRAINT [DF_tbl_AccountTrunk_Extend_U25]  DEFAULT (0x0FFFFFFF) FOR [U25]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk_Extend] ADD  CONSTRAINT [DF_tbl_AccountTrunk_Extend_R25]  DEFAULT (0xFF) FOR [R25]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk_Extend] ADD  CONSTRAINT [DF_tbl_AccountTrunk_Extend_S25]  DEFAULT ((0)) FOR [S25]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk_Extend] ADD  CONSTRAINT [DF_tbl_AccountTrunk_Extend_T25]  DEFAULT (0xFFFFFFFF) FOR [T25]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk_Extend] ADD  CONSTRAINT [DF_tbl_AccountTrunk_Extend_K26]  DEFAULT (0xFFFFFFFF) FOR [K26]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk_Extend] ADD  CONSTRAINT [DF_tbl_AccountTrunk_Extend_D26]  DEFAULT ((0)) FOR [D26]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk_Extend] ADD  CONSTRAINT [DF_tbl_AccountTrunk_Extend_U26]  DEFAULT (0x0FFFFFFF) FOR [U26]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk_Extend] ADD  CONSTRAINT [DF_tbl_AccountTrunk_Extend_R26]  DEFAULT (0xFF) FOR [R26]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk_Extend] ADD  CONSTRAINT [DF_tbl_AccountTrunk_Extend_S26]  DEFAULT ((0)) FOR [S26]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk_Extend] ADD  CONSTRAINT [DF_tbl_AccountTrunk_Extend_T26]  DEFAULT (0xFFFFFFFF) FOR [T26]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk_Extend] ADD  CONSTRAINT [DF_tbl_AccountTrunk_Extend_K27]  DEFAULT (0xFFFFFFFF) FOR [K27]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk_Extend] ADD  CONSTRAINT [DF_tbl_AccountTrunk_Extend_D27]  DEFAULT ((0)) FOR [D27]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk_Extend] ADD  CONSTRAINT [DF_tbl_AccountTrunk_Extend_U27]  DEFAULT (0x0FFFFFFF) FOR [U27]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk_Extend] ADD  CONSTRAINT [DF_tbl_AccountTrunk_Extend_R27]  DEFAULT (0xFF) FOR [R27]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk_Extend] ADD  CONSTRAINT [DF_tbl_AccountTrunk_Extend_S27]  DEFAULT ((0)) FOR [S27]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk_Extend] ADD  CONSTRAINT [DF_tbl_AccountTrunk_Extend_T27]  DEFAULT (0xFFFFFFFF) FOR [T27]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk_Extend] ADD  CONSTRAINT [DF_tbl_AccountTrunk_Extend_K28]  DEFAULT (0xFFFFFFFF) FOR [K28]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk_Extend] ADD  CONSTRAINT [DF_tbl_AccountTrunk_Extend_D28]  DEFAULT ((0)) FOR [D28]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk_Extend] ADD  CONSTRAINT [DF_tbl_AccountTrunk_Extend_U28]  DEFAULT (0x0FFFFFFF) FOR [U28]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk_Extend] ADD  CONSTRAINT [DF_tbl_AccountTrunk_Extend_R28]  DEFAULT (0xFF) FOR [R28]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk_Extend] ADD  CONSTRAINT [DF_tbl_AccountTrunk_Extend_S28]  DEFAULT ((0)) FOR [S28]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk_Extend] ADD  CONSTRAINT [DF_tbl_AccountTrunk_Extend_T28]  DEFAULT (0xFFFFFFFF) FOR [T28]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk_Extend] ADD  CONSTRAINT [DF_tbl_AccountTrunk_Extend_K29]  DEFAULT (0xFFFFFFFF) FOR [K29]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk_Extend] ADD  CONSTRAINT [DF_tbl_AccountTrunk_Extend_D29]  DEFAULT ((0)) FOR [D29]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk_Extend] ADD  CONSTRAINT [DF_tbl_AccountTrunk_Extend_U29]  DEFAULT (0x0FFFFFFF) FOR [U29]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk_Extend] ADD  CONSTRAINT [DF_tbl_AccountTrunk_Extend_R29]  DEFAULT (0xFF) FOR [R29]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk_Extend] ADD  CONSTRAINT [DF_tbl_AccountTrunk_Extend_S29]  DEFAULT ((0)) FOR [S29]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk_Extend] ADD  CONSTRAINT [DF_tbl_AccountTrunk_Extend_T29]  DEFAULT (0xFFFFFFFF) FOR [T29]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk_Extend] ADD  CONSTRAINT [DF_tbl_AccountTrunk_Extend_K30]  DEFAULT (0xFFFFFFFF) FOR [K30]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk_Extend] ADD  CONSTRAINT [DF_tbl_AccountTrunk_Extend_D30]  DEFAULT ((0)) FOR [D30]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk_Extend] ADD  CONSTRAINT [DF_tbl_AccountTrunk_Extend_U30]  DEFAULT (0x0FFFFFFF) FOR [U30]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk_Extend] ADD  CONSTRAINT [DF_tbl_AccountTrunk_Extend_R30]  DEFAULT (0xFF) FOR [R30]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk_Extend] ADD  CONSTRAINT [DF_tbl_AccountTrunk_Extend_S30]  DEFAULT ((0)) FOR [S30]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk_Extend] ADD  CONSTRAINT [DF_tbl_AccountTrunk_Extend_T30]  DEFAULT (0xFFFFFFFF) FOR [T30]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk_Extend] ADD  CONSTRAINT [DF_tbl_AccountTrunk_Extend_K31]  DEFAULT (0xFFFFFFFF) FOR [K31]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk_Extend] ADD  CONSTRAINT [DF_tbl_AccountTrunk_Extend_D31]  DEFAULT ((0)) FOR [D31]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk_Extend] ADD  CONSTRAINT [DF_tbl_AccountTrunk_Extend_U31]  DEFAULT (0x0FFFFFFF) FOR [U31]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk_Extend] ADD  CONSTRAINT [DF_tbl_AccountTrunk_Extend_R31]  DEFAULT (0xFF) FOR [R31]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk_Extend] ADD  CONSTRAINT [DF_tbl_AccountTrunk_Extend_S31]  DEFAULT ((0)) FOR [S31]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk_Extend] ADD  CONSTRAINT [DF_tbl_AccountTrunk_Extend_T31]  DEFAULT (0xFFFFFFFF) FOR [T31]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk_Extend] ADD  CONSTRAINT [DF_tbl_AccountTrunk_Extend_K32]  DEFAULT (0xFFFFFFFF) FOR [K32]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk_Extend] ADD  CONSTRAINT [DF_tbl_AccountTrunk_Extend_D32]  DEFAULT ((0)) FOR [D32]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk_Extend] ADD  CONSTRAINT [DF_tbl_AccountTrunk_Extend_U32]  DEFAULT (0x0FFFFFFF) FOR [U32]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk_Extend] ADD  CONSTRAINT [DF_tbl_AccountTrunk_Extend_R32]  DEFAULT (0xFF) FOR [R32]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk_Extend] ADD  CONSTRAINT [DF_tbl_AccountTrunk_Extend_S32]  DEFAULT ((0)) FOR [S32]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk_Extend] ADD  CONSTRAINT [DF_tbl_AccountTrunk_Extend_T32]  DEFAULT (0xFFFFFFFF) FOR [T32]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk_Extend] ADD  CONSTRAINT [DF_tbl_AccountTrunk_Extend_K33]  DEFAULT (0xFFFFFFFF) FOR [K33]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk_Extend] ADD  CONSTRAINT [DF_tbl_AccountTrunk_Extend_D33]  DEFAULT ((0)) FOR [D33]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk_Extend] ADD  CONSTRAINT [DF_tbl_AccountTrunk_Extend_U33]  DEFAULT (0x0FFFFFFF) FOR [U33]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk_Extend] ADD  CONSTRAINT [DF_tbl_AccountTrunk_Extend_R33]  DEFAULT (0xFF) FOR [R33]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk_Extend] ADD  CONSTRAINT [DF_tbl_AccountTrunk_Extend_S33]  DEFAULT ((0)) FOR [S33]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk_Extend] ADD  CONSTRAINT [DF_tbl_AccountTrunk_Extend_T33]  DEFAULT (0xFFFFFFFF) FOR [T33]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk_Extend] ADD  CONSTRAINT [DF_tbl_AccountTrunk_Extend_K34]  DEFAULT (0xFFFFFFFF) FOR [K34]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk_Extend] ADD  CONSTRAINT [DF_tbl_AccountTrunk_Extend_D34]  DEFAULT ((0)) FOR [D34]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk_Extend] ADD  CONSTRAINT [DF_tbl_AccountTrunk_Extend_U34]  DEFAULT (0x0FFFFFFF) FOR [U34]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk_Extend] ADD  CONSTRAINT [DF_tbl_AccountTrunk_Extend_R34]  DEFAULT (0xFF) FOR [R34]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk_Extend] ADD  CONSTRAINT [DF_tbl_AccountTrunk_Extend_S34]  DEFAULT ((0)) FOR [S34]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk_Extend] ADD  CONSTRAINT [DF_tbl_AccountTrunk_Extend_T34]  DEFAULT (0xFFFFFFFF) FOR [T34]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk_Extend] ADD  CONSTRAINT [DF_tbl_AccountTrunk_Extend_K35]  DEFAULT (0xFFFFFFFF) FOR [K35]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk_Extend] ADD  CONSTRAINT [DF_tbl_AccountTrunk_Extend_D35]  DEFAULT ((0)) FOR [D35]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk_Extend] ADD  CONSTRAINT [DF_tbl_AccountTrunk_Extend_U35]  DEFAULT (0x0FFFFFFF) FOR [U35]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk_Extend] ADD  CONSTRAINT [DF_tbl_AccountTrunk_Extend_R35]  DEFAULT (0xFF) FOR [R35]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk_Extend] ADD  CONSTRAINT [DF_tbl_AccountTrunk_Extend_S35]  DEFAULT ((0)) FOR [S35]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk_Extend] ADD  CONSTRAINT [DF_tbl_AccountTrunk_Extend_T35]  DEFAULT (0xFFFFFFFF) FOR [T35]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk_Extend] ADD  CONSTRAINT [DF_tbl_AccountTrunk_Extend_K36]  DEFAULT (0xFFFFFFFF) FOR [K36]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk_Extend] ADD  CONSTRAINT [DF_tbl_AccountTrunk_Extend_D36]  DEFAULT ((0)) FOR [D36]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk_Extend] ADD  CONSTRAINT [DF_tbl_AccountTrunk_Extend_U36]  DEFAULT (0x0FFFFFFF) FOR [U36]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk_Extend] ADD  CONSTRAINT [DF_tbl_AccountTrunk_Extend_R36]  DEFAULT (0xFF) FOR [R36]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk_Extend] ADD  CONSTRAINT [DF_tbl_AccountTrunk_Extend_S36]  DEFAULT ((0)) FOR [S36]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk_Extend] ADD  CONSTRAINT [DF_tbl_AccountTrunk_Extend_T36]  DEFAULT (0xFFFFFFFF) FOR [T36]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk_Extend] ADD  CONSTRAINT [DF_tbl_AccountTrunk_Extend_K37]  DEFAULT (0xFFFFFFFF) FOR [K37]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk_Extend] ADD  CONSTRAINT [DF_tbl_AccountTrunk_Extend_D37]  DEFAULT ((0)) FOR [D37]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk_Extend] ADD  CONSTRAINT [DF_tbl_AccountTrunk_Extend_U37]  DEFAULT (0x0FFFFFFF) FOR [U37]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk_Extend] ADD  CONSTRAINT [DF_tbl_AccountTrunk_Extend_R37]  DEFAULT (0xFF) FOR [R37]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk_Extend] ADD  CONSTRAINT [DF_tbl_AccountTrunk_Extend_S37]  DEFAULT ((0)) FOR [S37]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk_Extend] ADD  CONSTRAINT [DF_tbl_AccountTrunk_Extend_T37]  DEFAULT (0xFFFFFFFF) FOR [T37]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk_Extend] ADD  CONSTRAINT [DF_tbl_AccountTrunk_Extend_K38]  DEFAULT (0xFFFFFFFF) FOR [K38]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk_Extend] ADD  CONSTRAINT [DF_tbl_AccountTrunk_Extend_D38]  DEFAULT ((0)) FOR [D38]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk_Extend] ADD  CONSTRAINT [DF_tbl_AccountTrunk_Extend_U38]  DEFAULT (0x0FFFFFFF) FOR [U38]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk_Extend] ADD  CONSTRAINT [DF_tbl_AccountTrunk_Extend_R38]  DEFAULT (0xFF) FOR [R38]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk_Extend] ADD  CONSTRAINT [DF_tbl_AccountTrunk_Extend_S38]  DEFAULT ((0)) FOR [S38]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk_Extend] ADD  CONSTRAINT [DF_tbl_AccountTrunk_Extend_T38]  DEFAULT (0xFFFFFFFF) FOR [T38]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk_Extend] ADD  CONSTRAINT [DF_tbl_AccountTrunk_Extend_K39]  DEFAULT (0xFFFFFFFF) FOR [K39]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk_Extend] ADD  CONSTRAINT [DF_tbl_AccountTrunk_Extend_D39]  DEFAULT ((0)) FOR [D39]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk_Extend] ADD  CONSTRAINT [DF_tbl_AccountTrunk_Extend_U39]  DEFAULT (0x0FFFFFFF) FOR [U39]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk_Extend] ADD  CONSTRAINT [DF_tbl_AccountTrunk_Extend_R39]  DEFAULT (0xFF) FOR [R39]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk_Extend] ADD  CONSTRAINT [DF_tbl_AccountTrunk_Extend_S39]  DEFAULT ((0)) FOR [S39]
GO
ALTER TABLE [dbo].[tbl_AccountTrunk_Extend] ADD  CONSTRAINT [DF_tbl_AccountTrunk_Extend_T39]  DEFAULT (0xFFFFFFFF) FOR [T39]
GO
ALTER TABLE [dbo].[tbl_AccountTrunkCharge] ADD  CONSTRAINT [DF_tbl_AccountTrunkCharge_DCK]  DEFAULT ((0)) FOR [DCK]
GO
ALTER TABLE [dbo].[tbl_AccountTrunkCharge] ADD  CONSTRAINT [DF_tbl_AccountTrunkCharge_Money]  DEFAULT ((0)) FOR [Money]
GO
ALTER TABLE [dbo].[tbl_AccountTrunkCharge] ADD  CONSTRAINT [DF_tbl_AccountTrunkCharge_K]  DEFAULT (0xFFFFFFFF) FOR [K]
GO
ALTER TABLE [dbo].[tbl_AccountTrunkCharge] ADD  CONSTRAINT [DF_tbl_AccountTrunkCharge_D]  DEFAULT ((0)) FOR [D]
GO
ALTER TABLE [dbo].[tbl_AccountTrunkCharge] ADD  CONSTRAINT [DF_tbl_AccountTrunkCharge_U]  DEFAULT (0x0FFFFFFF) FOR [U]
GO
ALTER TABLE [dbo].[tbl_AccountTrunkCharge] ADD  CONSTRAINT [DF_tbl_AccountTrunkCharge_R]  DEFAULT (0xFF) FOR [R]
GO
ALTER TABLE [dbo].[tbl_AccountTrunkCharge] ADD  CONSTRAINT [DF_tbl_AccountTrunkCharge_GiveDate]  DEFAULT (getdate()) FOR [GiveDate]
GO
ALTER TABLE [dbo].[tbl_AccountTrunkCharge] ADD  CONSTRAINT [DF_tbl_AccountTrunkCharge_TakeDate]  DEFAULT ((0)) FOR [TakeDate]
GO
ALTER TABLE [dbo].[tbl_AccountTrunkCharge] ADD  CONSTRAINT [DF__tbl_accounttr__S__64CAC273]  DEFAULT ((0)) FOR [S]
GO
ALTER TABLE [dbo].[tbl_AccountTrunkCharge] ADD  CONSTRAINT [DF__tbl_accounttr__T__65BEE6AC]  DEFAULT (0xFFFFFFFF) FOR [T]
GO
ALTER TABLE [dbo].[tbl_aminepersonal_inven] ADD  CONSTRAINT [DF_tbl_aminepersonal_inven_avatorserial]  DEFAULT (0xFFFFFFFF) FOR [avatorserial]
GO
ALTER TABLE [dbo].[tbl_aminepersonal_inven] ADD  CONSTRAINT [DF_tbl_aminepersonal_inven_dck]  DEFAULT ((0)) FOR [dck]
GO
ALTER TABLE [dbo].[tbl_aminepersonal_inven] ADD  CONSTRAINT [DF_tbl_aminepersonal_inven_tmReg]  DEFAULT (getdate()) FOR [tmReg]
GO
ALTER TABLE [dbo].[tbl_aminepersonal_inven] ADD  CONSTRAINT [DF_tbl_aminepersonal_inven_tmUpdate]  DEFAULT (getdate()) FOR [tmUpdate]
GO
ALTER TABLE [dbo].[tbl_aminepersonal_inven] ADD  CONSTRAINT [DF_tbl_aminepersonal_K0]  DEFAULT (0xFFFFFFFF) FOR [K0]
GO
ALTER TABLE [dbo].[tbl_aminepersonal_inven] ADD  CONSTRAINT [DF_tbl_aminepersonal_inven_N0]  DEFAULT ((0)) FOR [N0]
GO
ALTER TABLE [dbo].[tbl_aminepersonal_inven] ADD  CONSTRAINT [DF_tbl_aminepersonal_K1]  DEFAULT (0xFFFFFFFF) FOR [K1]
GO
ALTER TABLE [dbo].[tbl_aminepersonal_inven] ADD  CONSTRAINT [DF_tbl_aminepersonal_inven_N1]  DEFAULT ((0)) FOR [N1]
GO
ALTER TABLE [dbo].[tbl_aminepersonal_inven] ADD  CONSTRAINT [DF_tbl_aminepersonal_K2]  DEFAULT (0xFFFFFFFF) FOR [K2]
GO
ALTER TABLE [dbo].[tbl_aminepersonal_inven] ADD  CONSTRAINT [DF_tbl_aminepersonal_inven_N2]  DEFAULT ((0)) FOR [N2]
GO
ALTER TABLE [dbo].[tbl_aminepersonal_inven] ADD  CONSTRAINT [DF_tbl_aminepersonal_K3]  DEFAULT (0xFFFFFFFF) FOR [K3]
GO
ALTER TABLE [dbo].[tbl_aminepersonal_inven] ADD  CONSTRAINT [DF_tbl_aminepersonal_inven_N3]  DEFAULT ((0)) FOR [N3]
GO
ALTER TABLE [dbo].[tbl_aminepersonal_inven] ADD  CONSTRAINT [DF_tbl_aminepersonal_K4]  DEFAULT (0xFFFFFFFF) FOR [K4]
GO
ALTER TABLE [dbo].[tbl_aminepersonal_inven] ADD  CONSTRAINT [DF_tbl_aminepersonal_inven_N4]  DEFAULT ((0)) FOR [N4]
GO
ALTER TABLE [dbo].[tbl_aminepersonal_inven] ADD  CONSTRAINT [DF_tbl_aminepersonal_K5]  DEFAULT (0xFFFFFFFF) FOR [K5]
GO
ALTER TABLE [dbo].[tbl_aminepersonal_inven] ADD  CONSTRAINT [DF_tbl_aminepersonal_inven_N5]  DEFAULT ((0)) FOR [N5]
GO
ALTER TABLE [dbo].[tbl_aminepersonal_inven] ADD  CONSTRAINT [DF_tbl_aminepersonal_K6]  DEFAULT (0xFFFFFFFF) FOR [K6]
GO
ALTER TABLE [dbo].[tbl_aminepersonal_inven] ADD  CONSTRAINT [DF_tbl_aminepersonal_inven_N6]  DEFAULT ((0)) FOR [N6]
GO
ALTER TABLE [dbo].[tbl_aminepersonal_inven] ADD  CONSTRAINT [DF_tbl_aminepersonal_K7]  DEFAULT (0xFFFFFFFF) FOR [K7]
GO
ALTER TABLE [dbo].[tbl_aminepersonal_inven] ADD  CONSTRAINT [DF_tbl_aminepersonal_inven_N7]  DEFAULT ((0)) FOR [N7]
GO
ALTER TABLE [dbo].[tbl_aminepersonal_inven] ADD  CONSTRAINT [DF_tbl_aminepersonal_K8]  DEFAULT (0xFFFFFFFF) FOR [K8]
GO
ALTER TABLE [dbo].[tbl_aminepersonal_inven] ADD  CONSTRAINT [DF_tbl_aminepersonal_inven_N8]  DEFAULT ((0)) FOR [N8]
GO
ALTER TABLE [dbo].[tbl_aminepersonal_inven] ADD  CONSTRAINT [DF_tbl_aminepersonal_K9]  DEFAULT (0xFFFFFFFF) FOR [K9]
GO
ALTER TABLE [dbo].[tbl_aminepersonal_inven] ADD  CONSTRAINT [DF_tbl_aminepersonal_inven_N9]  DEFAULT ((0)) FOR [N9]
GO
ALTER TABLE [dbo].[tbl_aminepersonal_inven] ADD  CONSTRAINT [DF_tbl_aminepersonal_K10]  DEFAULT (0xFFFFFFFF) FOR [K10]
GO
ALTER TABLE [dbo].[tbl_aminepersonal_inven] ADD  CONSTRAINT [DF_tbl_aminepersonal_inven_N10]  DEFAULT ((0)) FOR [N10]
GO
ALTER TABLE [dbo].[tbl_aminepersonal_inven] ADD  CONSTRAINT [DF_tbl_aminepersonal_K11]  DEFAULT (0xFFFFFFFF) FOR [K11]
GO
ALTER TABLE [dbo].[tbl_aminepersonal_inven] ADD  CONSTRAINT [DF_tbl_aminepersonal_inven_N11]  DEFAULT ((0)) FOR [N11]
GO
ALTER TABLE [dbo].[tbl_aminepersonal_inven] ADD  CONSTRAINT [DF_tbl_aminepersonal_K12]  DEFAULT (0xFFFFFFFF) FOR [K12]
GO
ALTER TABLE [dbo].[tbl_aminepersonal_inven] ADD  CONSTRAINT [DF_tbl_aminepersonal_inven_N12]  DEFAULT ((0)) FOR [N12]
GO
ALTER TABLE [dbo].[tbl_aminepersonal_inven] ADD  CONSTRAINT [DF_tbl_aminepersonal_K13]  DEFAULT (0xFFFFFFFF) FOR [K13]
GO
ALTER TABLE [dbo].[tbl_aminepersonal_inven] ADD  CONSTRAINT [DF_tbl_aminepersonal_inven_N13]  DEFAULT ((0)) FOR [N13]
GO
ALTER TABLE [dbo].[tbl_aminepersonal_inven] ADD  CONSTRAINT [DF_tbl_aminepersonal_K14]  DEFAULT (0xFFFFFFFF) FOR [K14]
GO
ALTER TABLE [dbo].[tbl_aminepersonal_inven] ADD  CONSTRAINT [DF_tbl_aminepersonal_inven_N14]  DEFAULT ((0)) FOR [N14]
GO
ALTER TABLE [dbo].[tbl_aminepersonal_inven] ADD  CONSTRAINT [DF_tbl_aminepersonal_K15]  DEFAULT (0xFFFFFFFF) FOR [K15]
GO
ALTER TABLE [dbo].[tbl_aminepersonal_inven] ADD  CONSTRAINT [DF_tbl_aminepersonal_inven_N15]  DEFAULT ((0)) FOR [N15]
GO
ALTER TABLE [dbo].[tbl_aminepersonal_inven] ADD  CONSTRAINT [DF_tbl_aminepersonal_K16]  DEFAULT (0xFFFFFFFF) FOR [K16]
GO
ALTER TABLE [dbo].[tbl_aminepersonal_inven] ADD  CONSTRAINT [DF_tbl_aminepersonal_inven_N16]  DEFAULT ((0)) FOR [N16]
GO
ALTER TABLE [dbo].[tbl_aminepersonal_inven] ADD  CONSTRAINT [DF_tbl_aminepersonal_K17]  DEFAULT (0xFFFFFFFF) FOR [K17]
GO
ALTER TABLE [dbo].[tbl_aminepersonal_inven] ADD  CONSTRAINT [DF_tbl_aminepersonal_inven_N17]  DEFAULT ((0)) FOR [N17]
GO
ALTER TABLE [dbo].[tbl_aminepersonal_inven] ADD  CONSTRAINT [DF_tbl_aminepersonal_K18]  DEFAULT (0xFFFFFFFF) FOR [K18]
GO
ALTER TABLE [dbo].[tbl_aminepersonal_inven] ADD  CONSTRAINT [DF_tbl_aminepersonal_inven_N18]  DEFAULT ((0)) FOR [N18]
GO
ALTER TABLE [dbo].[tbl_aminepersonal_inven] ADD  CONSTRAINT [DF_tbl_aminepersonal_K19]  DEFAULT (0xFFFFFFFF) FOR [K19]
GO
ALTER TABLE [dbo].[tbl_aminepersonal_inven] ADD  CONSTRAINT [DF_tbl_aminepersonal_inven_N19]  DEFAULT ((0)) FOR [N19]
GO
ALTER TABLE [dbo].[tbl_aminepersonal_inven] ADD  CONSTRAINT [DF_tbl_aminepersonal_K20]  DEFAULT (0xFFFFFFFF) FOR [K20]
GO
ALTER TABLE [dbo].[tbl_aminepersonal_inven] ADD  CONSTRAINT [DF_tbl_aminepersonal_inven_N20]  DEFAULT ((0)) FOR [N20]
GO
ALTER TABLE [dbo].[tbl_aminepersonal_inven] ADD  CONSTRAINT [DF_tbl_aminepersonal_K21]  DEFAULT (0xFFFFFFFF) FOR [K21]
GO
ALTER TABLE [dbo].[tbl_aminepersonal_inven] ADD  CONSTRAINT [DF_tbl_aminepersonal_inven_N21]  DEFAULT ((0)) FOR [N21]
GO
ALTER TABLE [dbo].[tbl_aminepersonal_inven] ADD  CONSTRAINT [DF_tbl_aminepersonal_K22]  DEFAULT (0xFFFFFFFF) FOR [K22]
GO
ALTER TABLE [dbo].[tbl_aminepersonal_inven] ADD  CONSTRAINT [DF_tbl_aminepersonal_inven_N22]  DEFAULT ((0)) FOR [N22]
GO
ALTER TABLE [dbo].[tbl_aminepersonal_inven] ADD  CONSTRAINT [DF_tbl_aminepersonal_K23]  DEFAULT (0xFFFFFFFF) FOR [K23]
GO
ALTER TABLE [dbo].[tbl_aminepersonal_inven] ADD  CONSTRAINT [DF_tbl_aminepersonal_inven_N23]  DEFAULT ((0)) FOR [N23]
GO
ALTER TABLE [dbo].[tbl_aminepersonal_inven] ADD  CONSTRAINT [DF_tbl_aminepersonal_K24]  DEFAULT (0xFFFFFFFF) FOR [K24]
GO
ALTER TABLE [dbo].[tbl_aminepersonal_inven] ADD  CONSTRAINT [DF_tbl_aminepersonal_inven_N24]  DEFAULT ((0)) FOR [N24]
GO
ALTER TABLE [dbo].[tbl_aminepersonal_inven] ADD  CONSTRAINT [DF_tbl_aminepersonal_K25]  DEFAULT (0xFFFFFFFF) FOR [K25]
GO
ALTER TABLE [dbo].[tbl_aminepersonal_inven] ADD  CONSTRAINT [DF_tbl_aminepersonal_inven_N25]  DEFAULT ((0)) FOR [N25]
GO
ALTER TABLE [dbo].[tbl_aminepersonal_inven] ADD  CONSTRAINT [DF_tbl_aminepersonal_K26]  DEFAULT (0xFFFFFFFF) FOR [K26]
GO
ALTER TABLE [dbo].[tbl_aminepersonal_inven] ADD  CONSTRAINT [DF_tbl_aminepersonal_inven_N26]  DEFAULT ((0)) FOR [N26]
GO
ALTER TABLE [dbo].[tbl_aminepersonal_inven] ADD  CONSTRAINT [DF_tbl_aminepersonal_K27]  DEFAULT (0xFFFFFFFF) FOR [K27]
GO
ALTER TABLE [dbo].[tbl_aminepersonal_inven] ADD  CONSTRAINT [DF_tbl_aminepersonal_inven_N27]  DEFAULT ((0)) FOR [N27]
GO
ALTER TABLE [dbo].[tbl_aminepersonal_inven] ADD  CONSTRAINT [DF_tbl_aminepersonal_K28]  DEFAULT (0xFFFFFFFF) FOR [K28]
GO
ALTER TABLE [dbo].[tbl_aminepersonal_inven] ADD  CONSTRAINT [DF_tbl_aminepersonal_inven_N28]  DEFAULT ((0)) FOR [N28]
GO
ALTER TABLE [dbo].[tbl_aminepersonal_inven] ADD  CONSTRAINT [DF_tbl_aminepersonal_K29]  DEFAULT (0xFFFFFFFF) FOR [K29]
GO
ALTER TABLE [dbo].[tbl_aminepersonal_inven] ADD  CONSTRAINT [DF_tbl_aminepersonal_inven_N29]  DEFAULT ((0)) FOR [N29]
GO
ALTER TABLE [dbo].[tbl_aminepersonal_inven] ADD  CONSTRAINT [DF_tbl_aminepersonal_K30]  DEFAULT (0xFFFFFFFF) FOR [K30]
GO
ALTER TABLE [dbo].[tbl_aminepersonal_inven] ADD  CONSTRAINT [DF_tbl_aminepersonal_inven_N30]  DEFAULT ((0)) FOR [N30]
GO
ALTER TABLE [dbo].[tbl_aminepersonal_inven] ADD  CONSTRAINT [DF_tbl_aminepersonal_K31]  DEFAULT (0xFFFFFFFF) FOR [K31]
GO
ALTER TABLE [dbo].[tbl_aminepersonal_inven] ADD  CONSTRAINT [DF_tbl_aminepersonal_inven_N31]  DEFAULT ((0)) FOR [N31]
GO
ALTER TABLE [dbo].[tbl_aminepersonal_inven] ADD  CONSTRAINT [DF_tbl_aminepersonal_K32]  DEFAULT (0xFFFFFFFF) FOR [K32]
GO
ALTER TABLE [dbo].[tbl_aminepersonal_inven] ADD  CONSTRAINT [DF_tbl_aminepersonal_inven_N32]  DEFAULT ((0)) FOR [N32]
GO
ALTER TABLE [dbo].[tbl_aminepersonal_inven] ADD  CONSTRAINT [DF_tbl_aminepersonal_K33]  DEFAULT (0xFFFFFFFF) FOR [K33]
GO
ALTER TABLE [dbo].[tbl_aminepersonal_inven] ADD  CONSTRAINT [DF_tbl_aminepersonal_inven_N33]  DEFAULT ((0)) FOR [N33]
GO
ALTER TABLE [dbo].[tbl_aminepersonal_inven] ADD  CONSTRAINT [DF_tbl_aminepersonal_K34]  DEFAULT (0xFFFFFFFF) FOR [K34]
GO
ALTER TABLE [dbo].[tbl_aminepersonal_inven] ADD  CONSTRAINT [DF_tbl_aminepersonal_inven_N34]  DEFAULT ((0)) FOR [N34]
GO
ALTER TABLE [dbo].[tbl_aminepersonal_inven] ADD  CONSTRAINT [DF_tbl_aminepersonal_K35]  DEFAULT (0xFFFFFFFF) FOR [K35]
GO
ALTER TABLE [dbo].[tbl_aminepersonal_inven] ADD  CONSTRAINT [DF_tbl_aminepersonal_inven_N35]  DEFAULT ((0)) FOR [N35]
GO
ALTER TABLE [dbo].[tbl_aminepersonal_inven] ADD  CONSTRAINT [DF_tbl_aminepersonal_K36]  DEFAULT (0xFFFFFFFF) FOR [K36]
GO
ALTER TABLE [dbo].[tbl_aminepersonal_inven] ADD  CONSTRAINT [DF_tbl_aminepersonal_inven_N36]  DEFAULT ((0)) FOR [N36]
GO
ALTER TABLE [dbo].[tbl_aminepersonal_inven] ADD  CONSTRAINT [DF_tbl_aminepersonal_K37]  DEFAULT (0xFFFFFFFF) FOR [K37]
GO
ALTER TABLE [dbo].[tbl_aminepersonal_inven] ADD  CONSTRAINT [DF_tbl_aminepersonal_inven_N37]  DEFAULT ((0)) FOR [N37]
GO
ALTER TABLE [dbo].[tbl_aminepersonal_inven] ADD  CONSTRAINT [DF_tbl_aminepersonal_K38]  DEFAULT (0xFFFFFFFF) FOR [K38]
GO
ALTER TABLE [dbo].[tbl_aminepersonal_inven] ADD  CONSTRAINT [DF_tbl_aminepersonal_inven_N38]  DEFAULT ((0)) FOR [N38]
GO
ALTER TABLE [dbo].[tbl_aminepersonal_inven] ADD  CONSTRAINT [DF_tbl_aminepersonal_K39]  DEFAULT (0xFFFFFFFF) FOR [K39]
GO
ALTER TABLE [dbo].[tbl_aminepersonal_inven] ADD  CONSTRAINT [DF_tbl_aminepersonal_inven_N39]  DEFAULT ((0)) FOR [N39]
GO
ALTER TABLE [dbo].[tbl_animusdata] ADD  CONSTRAINT [DF_tbl_animusdata_Serial]  DEFAULT (0xFFFFFFFF) FOR [Serial]
GO
ALTER TABLE [dbo].[tbl_animusdata] ADD  CONSTRAINT [DF_tbl_animusdata_Data0]  DEFAULT ((0)) FOR [Data0]
GO
ALTER TABLE [dbo].[tbl_animusdata] ADD  CONSTRAINT [DF_tbl_animusdata_Data1]  DEFAULT ((0)) FOR [Data1]
GO
ALTER TABLE [dbo].[tbl_animusdata] ADD  CONSTRAINT [DF_tbl_animusdata_Data2]  DEFAULT ((0)) FOR [Data2]
GO
ALTER TABLE [dbo].[tbl_animusdata] ADD  CONSTRAINT [DF_tbl_animusdata_Data3]  DEFAULT ((0)) FOR [Data3]
GO
ALTER TABLE [dbo].[tbl_animusdata] ADD  CONSTRAINT [DF_tbl_animusdata_Data4]  DEFAULT ((0)) FOR [Data4]
GO
ALTER TABLE [dbo].[tbl_animusdata] ADD  CONSTRAINT [DF_tbl_animusdata_Data5]  DEFAULT ((0)) FOR [Data5]
GO
ALTER TABLE [dbo].[tbl_animuslog] ADD  CONSTRAINT [DF_tbl_animuslog_Name]  DEFAULT ('*') FOR [Name]
GO
ALTER TABLE [dbo].[tbl_animuslog] ADD  CONSTRAINT [DF_tbl_animuslog_LogDate]  DEFAULT (getdate()) FOR [LogDate]
GO
ALTER TABLE [dbo].[tbl_ATradeTaxRate] ADD  CONSTRAINT [DF_tbl_ATradeTaxRate_GSerial]  DEFAULT ((-1)) FOR [GSerial]
GO
ALTER TABLE [dbo].[tbl_ATradeTaxRate] ADD  CONSTRAINT [DF_tbl_ATradeTaxRate_GName]  DEFAULT ('*') FOR [GName]
GO
ALTER TABLE [dbo].[tbl_ATradeTaxRate] ADD  CONSTRAINT [DF_tbl_ATradeTaxRate_Tax]  DEFAULT ((5)) FOR [Tax]
GO
ALTER TABLE [dbo].[tbl_ATradeTaxRate] ADD  CONSTRAINT [DF_tbl_ATradeTaxRate_NextTax]  DEFAULT ((5)) FOR [NextTax]
GO
ALTER TABLE [dbo].[tbl_ATradeTaxRate] ADD  CONSTRAINT [DF_tbl_ATradeTaxRate_UpdateTime]  DEFAULT (getdate()) FOR [UpdateTime]
GO
ALTER TABLE [dbo].[tbl_ATradeTaxRate] ADD  CONSTRAINT [DF__tbl_atrad__Sugge__566A9015]  DEFAULT (0xFFFFFFFF) FOR [Suggester]
GO
ALTER TABLE [dbo].[tbl_ATradeTaxRate] ADD  CONSTRAINT [DF__tbl_atrad__Sugge__575EB44E]  DEFAULT ('.') FOR [SuggesterName]
GO
ALTER TABLE [dbo].[tbl_automine_inven] ADD  CONSTRAINT [DF_tbl_automine_inven_dck]  DEFAULT ((0)) FOR [dck]
GO
ALTER TABLE [dbo].[tbl_automine_inven] ADD  CONSTRAINT [DF_tbl_automine_inven_Race]  DEFAULT (0xFF) FOR [Race]
GO
ALTER TABLE [dbo].[tbl_automine_inven] ADD  CONSTRAINT [DF_tbl_automine_inven_CollisionType]  DEFAULT (0xFF) FOR [CollisionType]
GO
ALTER TABLE [dbo].[tbl_automine_inven] ADD  CONSTRAINT [DF_tbl_automine_inven_GuildSerial]  DEFAULT (0xFFFFFFFF) FOR [GuildSerial]
GO
ALTER TABLE [dbo].[tbl_automine_inven] ADD  CONSTRAINT [DF_tbl_automine_inven_bWorking]  DEFAULT ((0)) FOR [bWorking]
GO
ALTER TABLE [dbo].[tbl_automine_inven] ADD  CONSTRAINT [DF_tbl_automine_inven_SelectOre]  DEFAULT ((0)) FOR [SelectOre]
GO
ALTER TABLE [dbo].[tbl_automine_inven] ADD  CONSTRAINT [DF_tbl_automine_inven_Battery]  DEFAULT ((0)) FOR [Battery]
GO
ALTER TABLE [dbo].[tbl_automine_inven] ADD  CONSTRAINT [DF_tbl_automine_inven_Reg]  DEFAULT (getdate()) FOR [tmReg]
GO
ALTER TABLE [dbo].[tbl_automine_inven] ADD  CONSTRAINT [DF_tbl_automine_inven_LastUpdate]  DEFAULT (getdate()) FOR [tmLastUpdate]
GO
ALTER TABLE [dbo].[tbl_automine_inven] ADD  CONSTRAINT [DF__tbl_automin__K_0]  DEFAULT (0xFFFFFFFF) FOR [K_0]
GO
ALTER TABLE [dbo].[tbl_automine_inven] ADD  CONSTRAINT [DF_tbl_automine_inven_O_0]  DEFAULT ((0)) FOR [O_0]
GO
ALTER TABLE [dbo].[tbl_automine_inven] ADD  CONSTRAINT [DF__tbl_automin__K_1]  DEFAULT (0xFFFFFFFF) FOR [K_1]
GO
ALTER TABLE [dbo].[tbl_automine_inven] ADD  CONSTRAINT [DF_tbl_automine_inven_O_1]  DEFAULT ((0)) FOR [O_1]
GO
ALTER TABLE [dbo].[tbl_automine_inven] ADD  CONSTRAINT [DF__tbl_automin__K_2]  DEFAULT (0xFFFFFFFF) FOR [K_2]
GO
ALTER TABLE [dbo].[tbl_automine_inven] ADD  CONSTRAINT [DF_tbl_automine_inven_O_2]  DEFAULT ((0)) FOR [O_2]
GO
ALTER TABLE [dbo].[tbl_automine_inven] ADD  CONSTRAINT [DF__tbl_automin__K_3]  DEFAULT (0xFFFFFFFF) FOR [K_3]
GO
ALTER TABLE [dbo].[tbl_automine_inven] ADD  CONSTRAINT [DF_tbl_automine_inven_O_3]  DEFAULT ((0)) FOR [O_3]
GO
ALTER TABLE [dbo].[tbl_automine_inven] ADD  CONSTRAINT [DF__tbl_automin__K_4]  DEFAULT (0xFFFFFFFF) FOR [K_4]
GO
ALTER TABLE [dbo].[tbl_automine_inven] ADD  CONSTRAINT [DF_tbl_automine_inven_O_4]  DEFAULT ((0)) FOR [O_4]
GO
ALTER TABLE [dbo].[tbl_automine_inven] ADD  CONSTRAINT [DF__tbl_automin__K_5]  DEFAULT (0xFFFFFFFF) FOR [K_5]
GO
ALTER TABLE [dbo].[tbl_automine_inven] ADD  CONSTRAINT [DF_tbl_automine_inven_O_5]  DEFAULT ((0)) FOR [O_5]
GO
ALTER TABLE [dbo].[tbl_automine_inven] ADD  CONSTRAINT [DF__tbl_automin__K_6]  DEFAULT (0xFFFFFFFF) FOR [K_6]
GO
ALTER TABLE [dbo].[tbl_automine_inven] ADD  CONSTRAINT [DF_tbl_automine_inven_O_6]  DEFAULT ((0)) FOR [O_6]
GO
ALTER TABLE [dbo].[tbl_automine_inven] ADD  CONSTRAINT [DF__tbl_automin__K_7]  DEFAULT (0xFFFFFFFF) FOR [K_7]
GO
ALTER TABLE [dbo].[tbl_automine_inven] ADD  CONSTRAINT [DF_tbl_automine_inven_O_7]  DEFAULT ((0)) FOR [O_7]
GO
ALTER TABLE [dbo].[tbl_automine_inven] ADD  CONSTRAINT [DF__tbl_automin__K_8]  DEFAULT (0xFFFFFFFF) FOR [K_8]
GO
ALTER TABLE [dbo].[tbl_automine_inven] ADD  CONSTRAINT [DF_tbl_automine_inven_O_8]  DEFAULT ((0)) FOR [O_8]
GO
ALTER TABLE [dbo].[tbl_automine_inven] ADD  CONSTRAINT [DF__tbl_automin__K_9]  DEFAULT (0xFFFFFFFF) FOR [K_9]
GO
ALTER TABLE [dbo].[tbl_automine_inven] ADD  CONSTRAINT [DF_tbl_automine_inven_O_9]  DEFAULT ((0)) FOR [O_9]
GO
ALTER TABLE [dbo].[tbl_automine_inven] ADD  CONSTRAINT [DF__tbl_automin__K_10]  DEFAULT (0xFFFFFFFF) FOR [K_10]
GO
ALTER TABLE [dbo].[tbl_automine_inven] ADD  CONSTRAINT [DF_tbl_automine_inven_O_10]  DEFAULT ((0)) FOR [O_10]
GO
ALTER TABLE [dbo].[tbl_automine_inven] ADD  CONSTRAINT [DF__tbl_automin__K_11]  DEFAULT (0xFFFFFFFF) FOR [K_11]
GO
ALTER TABLE [dbo].[tbl_automine_inven] ADD  CONSTRAINT [DF_tbl_automine_inven_O_11]  DEFAULT ((0)) FOR [O_11]
GO
ALTER TABLE [dbo].[tbl_automine_inven] ADD  CONSTRAINT [DF__tbl_automin__K_12]  DEFAULT (0xFFFFFFFF) FOR [K_12]
GO
ALTER TABLE [dbo].[tbl_automine_inven] ADD  CONSTRAINT [DF_tbl_automine_inven_O_12]  DEFAULT ((0)) FOR [O_12]
GO
ALTER TABLE [dbo].[tbl_automine_inven] ADD  CONSTRAINT [DF__tbl_automin__K_13]  DEFAULT (0xFFFFFFFF) FOR [K_13]
GO
ALTER TABLE [dbo].[tbl_automine_inven] ADD  CONSTRAINT [DF_tbl_automine_inven_O_13]  DEFAULT ((0)) FOR [O_13]
GO
ALTER TABLE [dbo].[tbl_automine_inven] ADD  CONSTRAINT [DF__tbl_automin__K_14]  DEFAULT (0xFFFFFFFF) FOR [K_14]
GO
ALTER TABLE [dbo].[tbl_automine_inven] ADD  CONSTRAINT [DF_tbl_automine_inven_O_14]  DEFAULT ((0)) FOR [O_14]
GO
ALTER TABLE [dbo].[tbl_automine_inven] ADD  CONSTRAINT [DF__tbl_automin__K_15]  DEFAULT (0xFFFFFFFF) FOR [K_15]
GO
ALTER TABLE [dbo].[tbl_automine_inven] ADD  CONSTRAINT [DF_tbl_automine_inven_O_15]  DEFAULT ((0)) FOR [O_15]
GO
ALTER TABLE [dbo].[tbl_automine_inven] ADD  CONSTRAINT [DF__tbl_automin__K_16]  DEFAULT (0xFFFFFFFF) FOR [K_16]
GO
ALTER TABLE [dbo].[tbl_automine_inven] ADD  CONSTRAINT [DF_tbl_automine_inven_O_16]  DEFAULT ((0)) FOR [O_16]
GO
ALTER TABLE [dbo].[tbl_automine_inven] ADD  CONSTRAINT [DF__tbl_automin__K_17]  DEFAULT (0xFFFFFFFF) FOR [K_17]
GO
ALTER TABLE [dbo].[tbl_automine_inven] ADD  CONSTRAINT [DF_tbl_automine_inven_O_17]  DEFAULT ((0)) FOR [O_17]
GO
ALTER TABLE [dbo].[tbl_automine_inven] ADD  CONSTRAINT [DF__tbl_automin__K_18]  DEFAULT (0xFFFFFFFF) FOR [K_18]
GO
ALTER TABLE [dbo].[tbl_automine_inven] ADD  CONSTRAINT [DF_tbl_automine_inven_O_18]  DEFAULT ((0)) FOR [O_18]
GO
ALTER TABLE [dbo].[tbl_automine_inven] ADD  CONSTRAINT [DF__tbl_automin__K_19]  DEFAULT (0xFFFFFFFF) FOR [K_19]
GO
ALTER TABLE [dbo].[tbl_automine_inven] ADD  CONSTRAINT [DF_tbl_automine_inven_O_19]  DEFAULT ((0)) FOR [O_19]
GO
ALTER TABLE [dbo].[tbl_automine_inven] ADD  CONSTRAINT [DF__tbl_automin__K_20]  DEFAULT (0xFFFFFFFF) FOR [K_20]
GO
ALTER TABLE [dbo].[tbl_automine_inven] ADD  CONSTRAINT [DF_tbl_automine_inven_O_20]  DEFAULT ((0)) FOR [O_20]
GO
ALTER TABLE [dbo].[tbl_automine_inven] ADD  CONSTRAINT [DF__tbl_automin__K_21]  DEFAULT (0xFFFFFFFF) FOR [K_21]
GO
ALTER TABLE [dbo].[tbl_automine_inven] ADD  CONSTRAINT [DF_tbl_automine_inven_O_21]  DEFAULT ((0)) FOR [O_21]
GO
ALTER TABLE [dbo].[tbl_automine_inven] ADD  CONSTRAINT [DF__tbl_automin__K_22]  DEFAULT (0xFFFFFFFF) FOR [K_22]
GO
ALTER TABLE [dbo].[tbl_automine_inven] ADD  CONSTRAINT [DF_tbl_automine_inven_O_22]  DEFAULT ((0)) FOR [O_22]
GO
ALTER TABLE [dbo].[tbl_automine_inven] ADD  CONSTRAINT [DF__tbl_automin__K_23]  DEFAULT (0xFFFFFFFF) FOR [K_23]
GO
ALTER TABLE [dbo].[tbl_automine_inven] ADD  CONSTRAINT [DF_tbl_automine_inven_O_23]  DEFAULT ((0)) FOR [O_23]
GO
ALTER TABLE [dbo].[tbl_automine_inven] ADD  CONSTRAINT [DF__tbl_automin__K_24]  DEFAULT (0xFFFFFFFF) FOR [K_24]
GO
ALTER TABLE [dbo].[tbl_automine_inven] ADD  CONSTRAINT [DF_tbl_automine_inven_O_24]  DEFAULT ((0)) FOR [O_24]
GO
ALTER TABLE [dbo].[tbl_automine_inven] ADD  CONSTRAINT [DF__tbl_automin__K_25]  DEFAULT (0xFFFFFFFF) FOR [K_25]
GO
ALTER TABLE [dbo].[tbl_automine_inven] ADD  CONSTRAINT [DF_tbl_automine_inven_O_25]  DEFAULT ((0)) FOR [O_25]
GO
ALTER TABLE [dbo].[tbl_automine_inven] ADD  CONSTRAINT [DF__tbl_automin__K_26]  DEFAULT (0xFFFFFFFF) FOR [K_26]
GO
ALTER TABLE [dbo].[tbl_automine_inven] ADD  CONSTRAINT [DF_tbl_automine_inven_O_26]  DEFAULT ((0)) FOR [O_26]
GO
ALTER TABLE [dbo].[tbl_automine_inven] ADD  CONSTRAINT [DF__tbl_automin__K_27]  DEFAULT (0xFFFFFFFF) FOR [K_27]
GO
ALTER TABLE [dbo].[tbl_automine_inven] ADD  CONSTRAINT [DF_tbl_automine_inven_O_27]  DEFAULT ((0)) FOR [O_27]
GO
ALTER TABLE [dbo].[tbl_automine_inven] ADD  CONSTRAINT [DF__tbl_automin__K_28]  DEFAULT (0xFFFFFFFF) FOR [K_28]
GO
ALTER TABLE [dbo].[tbl_automine_inven] ADD  CONSTRAINT [DF_tbl_automine_inven_O_28]  DEFAULT ((0)) FOR [O_28]
GO
ALTER TABLE [dbo].[tbl_automine_inven] ADD  CONSTRAINT [DF__tbl_automin__K_29]  DEFAULT (0xFFFFFFFF) FOR [K_29]
GO
ALTER TABLE [dbo].[tbl_automine_inven] ADD  CONSTRAINT [DF_tbl_automine_inven_O_29]  DEFAULT ((0)) FOR [O_29]
GO
ALTER TABLE [dbo].[tbl_automine_inven] ADD  CONSTRAINT [DF__tbl_automin__K_30]  DEFAULT (0xFFFFFFFF) FOR [K_30]
GO
ALTER TABLE [dbo].[tbl_automine_inven] ADD  CONSTRAINT [DF_tbl_automine_inven_O_30]  DEFAULT ((0)) FOR [O_30]
GO
ALTER TABLE [dbo].[tbl_automine_inven] ADD  CONSTRAINT [DF__tbl_automin__K_31]  DEFAULT (0xFFFFFFFF) FOR [K_31]
GO
ALTER TABLE [dbo].[tbl_automine_inven] ADD  CONSTRAINT [DF_tbl_automine_inven_O_31]  DEFAULT ((0)) FOR [O_31]
GO
ALTER TABLE [dbo].[tbl_automine_inven] ADD  CONSTRAINT [DF__tbl_automin__K_32]  DEFAULT (0xFFFFFFFF) FOR [K_32]
GO
ALTER TABLE [dbo].[tbl_automine_inven] ADD  CONSTRAINT [DF_tbl_automine_inven_O_32]  DEFAULT ((0)) FOR [O_32]
GO
ALTER TABLE [dbo].[tbl_automine_inven] ADD  CONSTRAINT [DF__tbl_automin__K_33]  DEFAULT (0xFFFFFFFF) FOR [K_33]
GO
ALTER TABLE [dbo].[tbl_automine_inven] ADD  CONSTRAINT [DF_tbl_automine_inven_O_33]  DEFAULT ((0)) FOR [O_33]
GO
ALTER TABLE [dbo].[tbl_automine_inven] ADD  CONSTRAINT [DF__tbl_automin__K_34]  DEFAULT (0xFFFFFFFF) FOR [K_34]
GO
ALTER TABLE [dbo].[tbl_automine_inven] ADD  CONSTRAINT [DF_tbl_automine_inven_O_34]  DEFAULT ((0)) FOR [O_34]
GO
ALTER TABLE [dbo].[tbl_automine_inven] ADD  CONSTRAINT [DF__tbl_automin__K_35]  DEFAULT (0xFFFFFFFF) FOR [K_35]
GO
ALTER TABLE [dbo].[tbl_automine_inven] ADD  CONSTRAINT [DF_tbl_automine_inven_O_35]  DEFAULT ((0)) FOR [O_35]
GO
ALTER TABLE [dbo].[tbl_automine_inven] ADD  CONSTRAINT [DF__tbl_automin__K_36]  DEFAULT (0xFFFFFFFF) FOR [K_36]
GO
ALTER TABLE [dbo].[tbl_automine_inven] ADD  CONSTRAINT [DF_tbl_automine_inven_O_36]  DEFAULT ((0)) FOR [O_36]
GO
ALTER TABLE [dbo].[tbl_automine_inven] ADD  CONSTRAINT [DF__tbl_automin__K_37]  DEFAULT (0xFFFFFFFF) FOR [K_37]
GO
ALTER TABLE [dbo].[tbl_automine_inven] ADD  CONSTRAINT [DF_tbl_automine_inven_O_37]  DEFAULT ((0)) FOR [O_37]
GO
ALTER TABLE [dbo].[tbl_automine_inven] ADD  CONSTRAINT [DF__tbl_automin__K_38]  DEFAULT (0xFFFFFFFF) FOR [K_38]
GO
ALTER TABLE [dbo].[tbl_automine_inven] ADD  CONSTRAINT [DF_tbl_automine_inven_O_38]  DEFAULT ((0)) FOR [O_38]
GO
ALTER TABLE [dbo].[tbl_automine_inven] ADD  CONSTRAINT [DF__tbl_automin__K_39]  DEFAULT (0xFFFFFFFF) FOR [K_39]
GO
ALTER TABLE [dbo].[tbl_automine_inven] ADD  CONSTRAINT [DF_tbl_automine_inven_O_39]  DEFAULT ((0)) FOR [O_39]
GO
ALTER TABLE [dbo].[tbl_automine_inven] ADD  CONSTRAINT [DF__tbl_automin__K_40]  DEFAULT (0xFFFFFFFF) FOR [K_40]
GO
ALTER TABLE [dbo].[tbl_automine_inven] ADD  CONSTRAINT [DF_tbl_automine_inven_O_40]  DEFAULT ((0)) FOR [O_40]
GO
ALTER TABLE [dbo].[tbl_automine_inven] ADD  CONSTRAINT [DF__tbl_automin__K_41]  DEFAULT (0xFFFFFFFF) FOR [K_41]
GO
ALTER TABLE [dbo].[tbl_automine_inven] ADD  CONSTRAINT [DF_tbl_automine_inven_O_41]  DEFAULT ((0)) FOR [O_41]
GO
ALTER TABLE [dbo].[tbl_automine_inven] ADD  CONSTRAINT [DF__tbl_automin__K_42]  DEFAULT (0xFFFFFFFF) FOR [K_42]
GO
ALTER TABLE [dbo].[tbl_automine_inven] ADD  CONSTRAINT [DF_tbl_automine_inven_O_42]  DEFAULT ((0)) FOR [O_42]
GO
ALTER TABLE [dbo].[tbl_automine_inven] ADD  CONSTRAINT [DF__tbl_automin__K_43]  DEFAULT (0xFFFFFFFF) FOR [K_43]
GO
ALTER TABLE [dbo].[tbl_automine_inven] ADD  CONSTRAINT [DF_tbl_automine_inven_O_43]  DEFAULT ((0)) FOR [O_43]
GO
ALTER TABLE [dbo].[tbl_automine_inven] ADD  CONSTRAINT [DF__tbl_automin__K_44]  DEFAULT (0xFFFFFFFF) FOR [K_44]
GO
ALTER TABLE [dbo].[tbl_automine_inven] ADD  CONSTRAINT [DF_tbl_automine_inven_O_44]  DEFAULT ((0)) FOR [O_44]
GO
ALTER TABLE [dbo].[tbl_automine_inven] ADD  CONSTRAINT [DF__tbl_automin__K_45]  DEFAULT (0xFFFFFFFF) FOR [K_45]
GO
ALTER TABLE [dbo].[tbl_automine_inven] ADD  CONSTRAINT [DF_tbl_automine_inven_O_45]  DEFAULT ((0)) FOR [O_45]
GO
ALTER TABLE [dbo].[tbl_automine_inven] ADD  CONSTRAINT [DF__tbl_automin__K_46]  DEFAULT (0xFFFFFFFF) FOR [K_46]
GO
ALTER TABLE [dbo].[tbl_automine_inven] ADD  CONSTRAINT [DF_tbl_automine_inven_O_46]  DEFAULT ((0)) FOR [O_46]
GO
ALTER TABLE [dbo].[tbl_automine_inven] ADD  CONSTRAINT [DF__tbl_automin__K_47]  DEFAULT (0xFFFFFFFF) FOR [K_47]
GO
ALTER TABLE [dbo].[tbl_automine_inven] ADD  CONSTRAINT [DF_tbl_automine_inven_O_47]  DEFAULT ((0)) FOR [O_47]
GO
ALTER TABLE [dbo].[tbl_automine_inven] ADD  CONSTRAINT [DF__tbl_automin__K_48]  DEFAULT (0xFFFFFFFF) FOR [K_48]
GO
ALTER TABLE [dbo].[tbl_automine_inven] ADD  CONSTRAINT [DF_tbl_automine_inven_O_48]  DEFAULT ((0)) FOR [O_48]
GO
ALTER TABLE [dbo].[tbl_automine_inven] ADD  CONSTRAINT [DF__tbl_automin__K_49]  DEFAULT (0xFFFFFFFF) FOR [K_49]
GO
ALTER TABLE [dbo].[tbl_automine_inven] ADD  CONSTRAINT [DF_tbl_automine_inven_O_49]  DEFAULT ((0)) FOR [O_49]
GO
ALTER TABLE [dbo].[tbl_automine_inven] ADD  CONSTRAINT [DF__tbl_automin__K_50]  DEFAULT (0xFFFFFFFF) FOR [K_50]
GO
ALTER TABLE [dbo].[tbl_automine_inven] ADD  CONSTRAINT [DF_tbl_automine_inven_O_50]  DEFAULT ((0)) FOR [O_50]
GO
ALTER TABLE [dbo].[tbl_automine_inven] ADD  CONSTRAINT [DF__tbl_automin__K_51]  DEFAULT (0xFFFFFFFF) FOR [K_51]
GO
ALTER TABLE [dbo].[tbl_automine_inven] ADD  CONSTRAINT [DF_tbl_automine_inven_O_51]  DEFAULT ((0)) FOR [O_51]
GO
ALTER TABLE [dbo].[tbl_automine_inven] ADD  CONSTRAINT [DF__tbl_automin__K_52]  DEFAULT (0xFFFFFFFF) FOR [K_52]
GO
ALTER TABLE [dbo].[tbl_automine_inven] ADD  CONSTRAINT [DF_tbl_automine_inven_O_52]  DEFAULT ((0)) FOR [O_52]
GO
ALTER TABLE [dbo].[tbl_automine_inven] ADD  CONSTRAINT [DF__tbl_automin__K_53]  DEFAULT (0xFFFFFFFF) FOR [K_53]
GO
ALTER TABLE [dbo].[tbl_automine_inven] ADD  CONSTRAINT [DF_tbl_automine_inven_O_53]  DEFAULT ((0)) FOR [O_53]
GO
ALTER TABLE [dbo].[tbl_automine_inven] ADD  CONSTRAINT [DF__tbl_automin__K_54]  DEFAULT (0xFFFFFFFF) FOR [K_54]
GO
ALTER TABLE [dbo].[tbl_automine_inven] ADD  CONSTRAINT [DF_tbl_automine_inven_O_54]  DEFAULT ((0)) FOR [O_54]
GO
ALTER TABLE [dbo].[tbl_automine_inven] ADD  CONSTRAINT [DF__tbl_automin__K_55]  DEFAULT (0xFFFFFFFF) FOR [K_55]
GO
ALTER TABLE [dbo].[tbl_automine_inven] ADD  CONSTRAINT [DF_tbl_automine_inven_O_55]  DEFAULT ((0)) FOR [O_55]
GO
ALTER TABLE [dbo].[tbl_automine_inven] ADD  CONSTRAINT [DF__tbl_automin__K_56]  DEFAULT (0xFFFFFFFF) FOR [K_56]
GO
ALTER TABLE [dbo].[tbl_automine_inven] ADD  CONSTRAINT [DF_tbl_automine_inven_O_56]  DEFAULT ((0)) FOR [O_56]
GO
ALTER TABLE [dbo].[tbl_automine_inven] ADD  CONSTRAINT [DF__tbl_automin__K_57]  DEFAULT (0xFFFFFFFF) FOR [K_57]
GO
ALTER TABLE [dbo].[tbl_automine_inven] ADD  CONSTRAINT [DF_tbl_automine_inven_O_57]  DEFAULT ((0)) FOR [O_57]
GO
ALTER TABLE [dbo].[tbl_automine_inven] ADD  CONSTRAINT [DF__tbl_automin__K_58]  DEFAULT (0xFFFFFFFF) FOR [K_58]
GO
ALTER TABLE [dbo].[tbl_automine_inven] ADD  CONSTRAINT [DF_tbl_automine_inven_O_58]  DEFAULT ((0)) FOR [O_58]
GO
ALTER TABLE [dbo].[tbl_automine_inven] ADD  CONSTRAINT [DF__tbl_automin__K_59]  DEFAULT (0xFFFFFFFF) FOR [K_59]
GO
ALTER TABLE [dbo].[tbl_automine_inven] ADD  CONSTRAINT [DF_tbl_automine_inven_O_59]  DEFAULT ((0)) FOR [O_59]
GO
ALTER TABLE [dbo].[tbl_automine_inven] ADD  CONSTRAINT [DF__tbl_automin__K_60]  DEFAULT (0xFFFFFFFF) FOR [K_60]
GO
ALTER TABLE [dbo].[tbl_automine_inven] ADD  CONSTRAINT [DF_tbl_automine_inven_O_60]  DEFAULT ((0)) FOR [O_60]
GO
ALTER TABLE [dbo].[tbl_automine_inven] ADD  CONSTRAINT [DF__tbl_automin__K_61]  DEFAULT (0xFFFFFFFF) FOR [K_61]
GO
ALTER TABLE [dbo].[tbl_automine_inven] ADD  CONSTRAINT [DF_tbl_automine_inven_O_61]  DEFAULT ((0)) FOR [O_61]
GO
ALTER TABLE [dbo].[tbl_automine_inven] ADD  CONSTRAINT [DF__tbl_automin__K_62]  DEFAULT (0xFFFFFFFF) FOR [K_62]
GO
ALTER TABLE [dbo].[tbl_automine_inven] ADD  CONSTRAINT [DF_tbl_automine_inven_O_62]  DEFAULT ((0)) FOR [O_62]
GO
ALTER TABLE [dbo].[tbl_automine_inven] ADD  CONSTRAINT [DF__tbl_automin__K_63]  DEFAULT (0xFFFFFFFF) FOR [K_63]
GO
ALTER TABLE [dbo].[tbl_automine_inven] ADD  CONSTRAINT [DF_tbl_automine_inven_O_63]  DEFAULT ((0)) FOR [O_63]
GO
ALTER TABLE [dbo].[tbl_automine_inven] ADD  CONSTRAINT [DF__tbl_automin__K_64]  DEFAULT (0xFFFFFFFF) FOR [K_64]
GO
ALTER TABLE [dbo].[tbl_automine_inven] ADD  CONSTRAINT [DF_tbl_automine_inven_O_64]  DEFAULT ((0)) FOR [O_64]
GO
ALTER TABLE [dbo].[tbl_automine_inven] ADD  CONSTRAINT [DF__tbl_automin__K_65]  DEFAULT (0xFFFFFFFF) FOR [K_65]
GO
ALTER TABLE [dbo].[tbl_automine_inven] ADD  CONSTRAINT [DF_tbl_automine_inven_O_65]  DEFAULT ((0)) FOR [O_65]
GO
ALTER TABLE [dbo].[tbl_automine_inven] ADD  CONSTRAINT [DF__tbl_automin__K_66]  DEFAULT (0xFFFFFFFF) FOR [K_66]
GO
ALTER TABLE [dbo].[tbl_automine_inven] ADD  CONSTRAINT [DF_tbl_automine_inven_O_66]  DEFAULT ((0)) FOR [O_66]
GO
ALTER TABLE [dbo].[tbl_automine_inven] ADD  CONSTRAINT [DF__tbl_automin__K_67]  DEFAULT (0xFFFFFFFF) FOR [K_67]
GO
ALTER TABLE [dbo].[tbl_automine_inven] ADD  CONSTRAINT [DF_tbl_automine_inven_O_67]  DEFAULT ((0)) FOR [O_67]
GO
ALTER TABLE [dbo].[tbl_automine_inven] ADD  CONSTRAINT [DF__tbl_automin__K_68]  DEFAULT (0xFFFFFFFF) FOR [K_68]
GO
ALTER TABLE [dbo].[tbl_automine_inven] ADD  CONSTRAINT [DF_tbl_automine_inven_O_68]  DEFAULT ((0)) FOR [O_68]
GO
ALTER TABLE [dbo].[tbl_automine_inven] ADD  CONSTRAINT [DF__tbl_automin__K_69]  DEFAULT (0xFFFFFFFF) FOR [K_69]
GO
ALTER TABLE [dbo].[tbl_automine_inven] ADD  CONSTRAINT [DF_tbl_automine_inven_O_69]  DEFAULT ((0)) FOR [O_69]
GO
ALTER TABLE [dbo].[tbl_automine_inven] ADD  CONSTRAINT [DF__tbl_automin__K_70]  DEFAULT (0xFFFFFFFF) FOR [K_70]
GO
ALTER TABLE [dbo].[tbl_automine_inven] ADD  CONSTRAINT [DF_tbl_automine_inven_O_70]  DEFAULT ((0)) FOR [O_70]
GO
ALTER TABLE [dbo].[tbl_automine_inven] ADD  CONSTRAINT [DF__tbl_automin__K_71]  DEFAULT (0xFFFFFFFF) FOR [K_71]
GO
ALTER TABLE [dbo].[tbl_automine_inven] ADD  CONSTRAINT [DF_tbl_automine_inven_O_71]  DEFAULT ((0)) FOR [O_71]
GO
ALTER TABLE [dbo].[tbl_automine_inven] ADD  CONSTRAINT [DF__tbl_automin__K_72]  DEFAULT (0xFFFFFFFF) FOR [K_72]
GO
ALTER TABLE [dbo].[tbl_automine_inven] ADD  CONSTRAINT [DF_tbl_automine_inven_O_72]  DEFAULT ((0)) FOR [O_72]
GO
ALTER TABLE [dbo].[tbl_automine_inven] ADD  CONSTRAINT [DF__tbl_automin__K_73]  DEFAULT (0xFFFFFFFF) FOR [K_73]
GO
ALTER TABLE [dbo].[tbl_automine_inven] ADD  CONSTRAINT [DF_tbl_automine_inven_O_73]  DEFAULT ((0)) FOR [O_73]
GO
ALTER TABLE [dbo].[tbl_automine_inven] ADD  CONSTRAINT [DF__tbl_automin__K_74]  DEFAULT (0xFFFFFFFF) FOR [K_74]
GO
ALTER TABLE [dbo].[tbl_automine_inven] ADD  CONSTRAINT [DF_tbl_automine_inven_O_74]  DEFAULT ((0)) FOR [O_74]
GO
ALTER TABLE [dbo].[tbl_automine_inven] ADD  CONSTRAINT [DF__tbl_automin__K_75]  DEFAULT (0xFFFFFFFF) FOR [K_75]
GO
ALTER TABLE [dbo].[tbl_automine_inven] ADD  CONSTRAINT [DF_tbl_automine_inven_O_75]  DEFAULT ((0)) FOR [O_75]
GO
ALTER TABLE [dbo].[tbl_automine_inven] ADD  CONSTRAINT [DF__tbl_automin__K_76]  DEFAULT (0xFFFFFFFF) FOR [K_76]
GO
ALTER TABLE [dbo].[tbl_automine_inven] ADD  CONSTRAINT [DF_tbl_automine_inven_O_76]  DEFAULT ((0)) FOR [O_76]
GO
ALTER TABLE [dbo].[tbl_automine_inven] ADD  CONSTRAINT [DF__tbl_automin__K_77]  DEFAULT (0xFFFFFFFF) FOR [K_77]
GO
ALTER TABLE [dbo].[tbl_automine_inven] ADD  CONSTRAINT [DF_tbl_automine_inven_O_77]  DEFAULT ((0)) FOR [O_77]
GO
ALTER TABLE [dbo].[tbl_automine_inven] ADD  CONSTRAINT [DF__tbl_automin__K_78]  DEFAULT (0xFFFFFFFF) FOR [K_78]
GO
ALTER TABLE [dbo].[tbl_automine_inven] ADD  CONSTRAINT [DF_tbl_automine_inven_O_78]  DEFAULT ((0)) FOR [O_78]
GO
ALTER TABLE [dbo].[tbl_automine_inven] ADD  CONSTRAINT [DF__tbl_automin__K_79]  DEFAULT (0xFFFFFFFF) FOR [K_79]
GO
ALTER TABLE [dbo].[tbl_automine_inven] ADD  CONSTRAINT [DF_tbl_automine_inven_O_79]  DEFAULT ((0)) FOR [O_79]
GO
ALTER TABLE [dbo].[tbl_base] ADD  CONSTRAINT [DF_tbl_base_DCK]  DEFAULT ((0)) FOR [DCK]
GO
ALTER TABLE [dbo].[tbl_base] ADD  CONSTRAINT [DF_tbl_base_Lock]  DEFAULT ((0)) FOR [Lock]
GO
ALTER TABLE [dbo].[tbl_base] ADD  CONSTRAINT [DF_tbl_base_Name]  DEFAULT ('*') FOR [Name]
GO
ALTER TABLE [dbo].[tbl_base] ADD  CONSTRAINT [DF_tbl_base_Account]  DEFAULT ((0)) FOR [AccountSerial]
GO
ALTER TABLE [dbo].[tbl_base] ADD  CONSTRAINT [DF_tbl_base_Account_1]  DEFAULT ('*') FOR [Account]
GO
ALTER TABLE [dbo].[tbl_base] ADD  CONSTRAINT [DF_tbl_base_Slot]  DEFAULT ((0)) FOR [Slot]
GO
ALTER TABLE [dbo].[tbl_base] ADD  CONSTRAINT [DF_tbl_base_Race]  DEFAULT ((0)) FOR [Race]
GO
ALTER TABLE [dbo].[tbl_base] ADD  CONSTRAINT [DF_tbl_base_Class_1]  DEFAULT ('000') FOR [Class]
GO
ALTER TABLE [dbo].[tbl_base] ADD  CONSTRAINT [DF_tbl_base_Lv]  DEFAULT ((0)) FOR [Lv]
GO
ALTER TABLE [dbo].[tbl_base] ADD  CONSTRAINT [DF_tbl_base_Dalant]  DEFAULT ((0)) FOR [Dalant]
GO
ALTER TABLE [dbo].[tbl_base] ADD  CONSTRAINT [DF_tbl_base_Dalant64]  DEFAULT ((0)) FOR [Dalant64]
GO
ALTER TABLE [dbo].[tbl_base] ADD  CONSTRAINT [DF_tbl_base_Gold]  DEFAULT ((0)) FOR [Gold]
GO
ALTER TABLE [dbo].[tbl_base] ADD  CONSTRAINT [DF_tbl_base_Gold64]  DEFAULT ((0)) FOR [Gold64]
GO
ALTER TABLE [dbo].[tbl_base] ADD  CONSTRAINT [DF_tbl_base_BaseShape]  DEFAULT (0x00000000) FOR [BaseShape]
GO
ALTER TABLE [dbo].[tbl_base] ADD  CONSTRAINT [DF_tbl_base_EK0]  DEFAULT (0xFFFFFFFF) FOR [EK0]
GO
ALTER TABLE [dbo].[tbl_base] ADD  CONSTRAINT [DF_tbl_base_EU0]  DEFAULT (0x0FFFFFFF) FOR [EU0]
GO
ALTER TABLE [dbo].[tbl_base] ADD  CONSTRAINT [DF_tbl_base_EK1]  DEFAULT (0xFFFFFFFF) FOR [EK1]
GO
ALTER TABLE [dbo].[tbl_base] ADD  CONSTRAINT [DF_tbl_base_EU1]  DEFAULT (0x0FFFFFFF) FOR [EU1]
GO
ALTER TABLE [dbo].[tbl_base] ADD  CONSTRAINT [DF_tbl_base_EK2]  DEFAULT (0xFFFFFFFF) FOR [EK2]
GO
ALTER TABLE [dbo].[tbl_base] ADD  CONSTRAINT [DF_tbl_base_EU2]  DEFAULT (0x0FFFFFFF) FOR [EU2]
GO
ALTER TABLE [dbo].[tbl_base] ADD  CONSTRAINT [DF_tbl_base_EK3]  DEFAULT (0xFFFFFFFF) FOR [EK3]
GO
ALTER TABLE [dbo].[tbl_base] ADD  CONSTRAINT [DF_tbl_base_EU3]  DEFAULT (0x0FFFFFFF) FOR [EU3]
GO
ALTER TABLE [dbo].[tbl_base] ADD  CONSTRAINT [DF_tbl_base_EK4]  DEFAULT (0xFFFFFFFF) FOR [EK4]
GO
ALTER TABLE [dbo].[tbl_base] ADD  CONSTRAINT [DF_tbl_base_EU4]  DEFAULT (0x0FFFFFFF) FOR [EU4]
GO
ALTER TABLE [dbo].[tbl_base] ADD  CONSTRAINT [DF_tbl_base_EK5]  DEFAULT (0xFFFFFFFF) FOR [EK5]
GO
ALTER TABLE [dbo].[tbl_base] ADD  CONSTRAINT [DF_tbl_base_EU5]  DEFAULT (0x0FFFFFFF) FOR [EU5]
GO
ALTER TABLE [dbo].[tbl_base] ADD  CONSTRAINT [DF_tbl_base_EK6]  DEFAULT (0xFFFFFFFF) FOR [EK6]
GO
ALTER TABLE [dbo].[tbl_base] ADD  CONSTRAINT [DF_tbl_base_EU6]  DEFAULT (0x0FFFFFFF) FOR [EU6]
GO
ALTER TABLE [dbo].[tbl_base] ADD  CONSTRAINT [DF_tbl_base_EK7]  DEFAULT (0xFFFFFFFF) FOR [EK7]
GO
ALTER TABLE [dbo].[tbl_base] ADD  CONSTRAINT [DF_tbl_base_EU7]  DEFAULT (0x0FFFFFFF) FOR [EU7]
GO
ALTER TABLE [dbo].[tbl_base] ADD  CONSTRAINT [DF_tbl_base_LastConnTime]  DEFAULT ((0)) FOR [LastConnTime]
GO
ALTER TABLE [dbo].[tbl_base] ADD  CONSTRAINT [DF_tbl_base_CreateTime_1]  DEFAULT (getdate()) FOR [CreateTime]
GO
ALTER TABLE [dbo].[tbl_base] ADD  CONSTRAINT [DF_tbl_base_DeleteTime]  DEFAULT ((0)) FOR [DeleteTime]
GO
ALTER TABLE [dbo].[tbl_base] ADD  CONSTRAINT [DF_tbl_base_DeleteName]  DEFAULT ('*') FOR [DeleteName]
GO
ALTER TABLE [dbo].[tbl_base] ADD  CONSTRAINT [DF_tbl_base_FirstConnTime]  DEFAULT ((0)) FOR [FirstConnTime]
GO
ALTER TABLE [dbo].[tbl_base] ADD  CONSTRAINT [DF__tbl_base__HomeSe__5031C87B]  DEFAULT ('*') FOR [HomeServer]
GO
ALTER TABLE [dbo].[tbl_base] ADD  CONSTRAINT [DF__tbl_base__Arrang__5125ECB4]  DEFAULT ((1)) FOR [Arrange]
GO
ALTER TABLE [dbo].[tbl_base] ADD  CONSTRAINT [DF__tbl_base__ES0__6787338D]  DEFAULT ((0)) FOR [ES0]
GO
ALTER TABLE [dbo].[tbl_base] ADD  CONSTRAINT [DF__tbl_base__ET0__687B57C6]  DEFAULT (0xFFFFFFFF) FOR [ET0]
GO
ALTER TABLE [dbo].[tbl_base] ADD  CONSTRAINT [DF__tbl_base__ES1__696F7BFF]  DEFAULT ((0)) FOR [ES1]
GO
ALTER TABLE [dbo].[tbl_base] ADD  CONSTRAINT [DF__tbl_base__ET1__6A63A038]  DEFAULT (0xFFFFFFFF) FOR [ET1]
GO
ALTER TABLE [dbo].[tbl_base] ADD  CONSTRAINT [DF__tbl_base__ES2__6B57C471]  DEFAULT ((0)) FOR [ES2]
GO
ALTER TABLE [dbo].[tbl_base] ADD  CONSTRAINT [DF__tbl_base__ET2__6C4BE8AA]  DEFAULT (0xFFFFFFFF) FOR [ET2]
GO
ALTER TABLE [dbo].[tbl_base] ADD  CONSTRAINT [DF__tbl_base__ES3__6D400CE3]  DEFAULT ((0)) FOR [ES3]
GO
ALTER TABLE [dbo].[tbl_base] ADD  CONSTRAINT [DF__tbl_base__ET3__6E34311C]  DEFAULT (0xFFFFFFFF) FOR [ET3]
GO
ALTER TABLE [dbo].[tbl_base] ADD  CONSTRAINT [DF__tbl_base__ES4__6F285555]  DEFAULT ((0)) FOR [ES4]
GO
ALTER TABLE [dbo].[tbl_base] ADD  CONSTRAINT [DF__tbl_base__ET4__701C798E]  DEFAULT (0xFFFFFFFF) FOR [ET4]
GO
ALTER TABLE [dbo].[tbl_base] ADD  CONSTRAINT [DF__tbl_base__ES5__71109DC7]  DEFAULT ((0)) FOR [ES5]
GO
ALTER TABLE [dbo].[tbl_base] ADD  CONSTRAINT [DF__tbl_base__ET5__7204C200]  DEFAULT (0xFFFFFFFF) FOR [ET5]
GO
ALTER TABLE [dbo].[tbl_base] ADD  CONSTRAINT [DF__tbl_base__ES6__72F8E639]  DEFAULT ((0)) FOR [ES6]
GO
ALTER TABLE [dbo].[tbl_base] ADD  CONSTRAINT [DF__tbl_base__ET6__73ED0A72]  DEFAULT (0xFFFFFFFF) FOR [ET6]
GO
ALTER TABLE [dbo].[tbl_base] ADD  CONSTRAINT [DF__tbl_base__ES7__74E12EAB]  DEFAULT ((0)) FOR [ES7]
GO
ALTER TABLE [dbo].[tbl_base] ADD  CONSTRAINT [DF__tbl_base__ET7__75D552E4]  DEFAULT (0xFFFFFFFF) FOR [ET7]
GO
ALTER TABLE [dbo].[tbl_base] ADD  CONSTRAINT [DF__tbl_base__EK8__7F0D7E4C]  DEFAULT ((-1)) FOR [EK8]
GO
ALTER TABLE [dbo].[tbl_base] ADD  CONSTRAINT [DF__tbl_base__EU8__0001A285]  DEFAULT (0x0FFFFFFF) FOR [EU8]
GO
ALTER TABLE [dbo].[tbl_base] ADD  CONSTRAINT [DF__tbl_base__ES8__00F5C6BE]  DEFAULT ((0)) FOR [ES8]
GO
ALTER TABLE [dbo].[tbl_base] ADD  CONSTRAINT [DF__tbl_base__ET8__01E9EAF7]  DEFAULT ((-1)) FOR [ET8]
GO
ALTER TABLE [dbo].[tbl_Buddy] ADD  CONSTRAINT [DF_tbl_Buddy_Serial]  DEFAULT (0xFFFFFFFF) FOR [Serial]
GO
ALTER TABLE [dbo].[tbl_Buddy] ADD  CONSTRAINT [DF_tbl_Buddy_Serial0]  DEFAULT (0xFFFFFFFF) FOR [Serial0]
GO
ALTER TABLE [dbo].[tbl_Buddy] ADD  CONSTRAINT [DF_tbl_Buddy_Serial1]  DEFAULT (0xFFFFFFFF) FOR [Serial1]
GO
ALTER TABLE [dbo].[tbl_Buddy] ADD  CONSTRAINT [DF_tbl_Buddy_Serial2]  DEFAULT (0xFFFFFFFF) FOR [Serial2]
GO
ALTER TABLE [dbo].[tbl_Buddy] ADD  CONSTRAINT [DF_tbl_Buddy_Serial3]  DEFAULT (0xFFFFFFFF) FOR [Serial3]
GO
ALTER TABLE [dbo].[tbl_Buddy] ADD  CONSTRAINT [DF_tbl_Buddy_Serial4]  DEFAULT (0xFFFFFFFF) FOR [Serial4]
GO
ALTER TABLE [dbo].[tbl_Buddy] ADD  CONSTRAINT [DF_tbl_Buddy_Serial5]  DEFAULT (0xFFFFFFFF) FOR [Serial5]
GO
ALTER TABLE [dbo].[tbl_Buddy] ADD  CONSTRAINT [DF_tbl_Buddy_Serial6]  DEFAULT (0xFFFFFFFF) FOR [Serial6]
GO
ALTER TABLE [dbo].[tbl_Buddy] ADD  CONSTRAINT [DF_tbl_Buddy_Serial7]  DEFAULT (0xFFFFFFFF) FOR [Serial7]
GO
ALTER TABLE [dbo].[tbl_Buddy] ADD  CONSTRAINT [DF_tbl_Buddy_Serial8]  DEFAULT (0xFFFFFFFF) FOR [Serial8]
GO
ALTER TABLE [dbo].[tbl_Buddy] ADD  CONSTRAINT [DF_tbl_Buddy_Serial9]  DEFAULT (0xFFFFFFFF) FOR [Serial9]
GO
ALTER TABLE [dbo].[tbl_Buddy] ADD  CONSTRAINT [DF_tbl_Buddy_Serial10]  DEFAULT (0xFFFFFFFF) FOR [Serial10]
GO
ALTER TABLE [dbo].[tbl_Buddy] ADD  CONSTRAINT [DF_tbl_Buddy_Serial11]  DEFAULT (0xFFFFFFFF) FOR [Serial11]
GO
ALTER TABLE [dbo].[tbl_Buddy] ADD  CONSTRAINT [DF_tbl_Buddy_Serial12]  DEFAULT (0xFFFFFFFF) FOR [Serial12]
GO
ALTER TABLE [dbo].[tbl_Buddy] ADD  CONSTRAINT [DF_tbl_Buddy_Serial13]  DEFAULT (0xFFFFFFFF) FOR [Serial13]
GO
ALTER TABLE [dbo].[tbl_Buddy] ADD  CONSTRAINT [DF_tbl_Buddy_Serial131]  DEFAULT (0xFFFFFFFF) FOR [Serial14]
GO
ALTER TABLE [dbo].[tbl_Buddy] ADD  CONSTRAINT [DF_tbl_Buddy_Serial141]  DEFAULT (0xFFFFFFFF) FOR [Serial15]
GO
ALTER TABLE [dbo].[tbl_Buddy] ADD  CONSTRAINT [DF_tbl_Buddy_Serial141_1]  DEFAULT (0xFFFFFFFF) FOR [Serial16]
GO
ALTER TABLE [dbo].[tbl_Buddy] ADD  CONSTRAINT [DF_tbl_Buddy_Serial161]  DEFAULT (0xFFFFFFFF) FOR [Serial17]
GO
ALTER TABLE [dbo].[tbl_Buddy] ADD  CONSTRAINT [DF_tbl_Buddy_Serial171]  DEFAULT (0xFFFFFFFF) FOR [Serial18]
GO
ALTER TABLE [dbo].[tbl_Buddy] ADD  CONSTRAINT [DF_tbl_Buddy_Serial171_1]  DEFAULT (0xFFFFFFFF) FOR [Serial19]
GO
ALTER TABLE [dbo].[tbl_Buddy] ADD  CONSTRAINT [DF_tbl_Buddy_Serial101]  DEFAULT (0xFFFFFFFF) FOR [Serial20]
GO
ALTER TABLE [dbo].[tbl_Buddy] ADD  CONSTRAINT [DF_tbl_Buddy_Serial111]  DEFAULT (0xFFFFFFFF) FOR [Serial21]
GO
ALTER TABLE [dbo].[tbl_Buddy] ADD  CONSTRAINT [DF_tbl_Buddy_Serial121]  DEFAULT (0xFFFFFFFF) FOR [Serial22]
GO
ALTER TABLE [dbo].[tbl_Buddy] ADD  CONSTRAINT [DF_tbl_Buddy_Serial131_1]  DEFAULT (0xFFFFFFFF) FOR [Serial23]
GO
ALTER TABLE [dbo].[tbl_Buddy] ADD  CONSTRAINT [DF_tbl_Buddy_Serial141_2]  DEFAULT (0xFFFFFFFF) FOR [Serial24]
GO
ALTER TABLE [dbo].[tbl_Buddy] ADD  CONSTRAINT [DF_tbl_Buddy_Serial151]  DEFAULT (0xFFFFFFFF) FOR [Serial25]
GO
ALTER TABLE [dbo].[tbl_Buddy] ADD  CONSTRAINT [DF_tbl_Buddy_Serial161_1]  DEFAULT (0xFFFFFFFF) FOR [Serial26]
GO
ALTER TABLE [dbo].[tbl_Buddy] ADD  CONSTRAINT [DF_tbl_Buddy_Serial171_2]  DEFAULT (0xFFFFFFFF) FOR [Serial27]
GO
ALTER TABLE [dbo].[tbl_Buddy] ADD  CONSTRAINT [DF_tbl_Buddy_Serial181]  DEFAULT (0xFFFFFFFF) FOR [Serial28]
GO
ALTER TABLE [dbo].[tbl_Buddy] ADD  CONSTRAINT [DF_tbl_Buddy_Serial191]  DEFAULT (0xFFFFFFFF) FOR [Serial29]
GO
ALTER TABLE [dbo].[tbl_Buddy] ADD  CONSTRAINT [DF_tbl_Buddy_Serial201]  DEFAULT (0xFFFFFFFF) FOR [Serial30]
GO
ALTER TABLE [dbo].[tbl_Buddy] ADD  CONSTRAINT [DF_tbl_Buddy_Serial211]  DEFAULT (0xFFFFFFFF) FOR [Serial31]
GO
ALTER TABLE [dbo].[tbl_Buddy] ADD  CONSTRAINT [DF_tbl_Buddy_Serial221]  DEFAULT (0xFFFFFFFF) FOR [Serial32]
GO
ALTER TABLE [dbo].[tbl_Buddy] ADD  CONSTRAINT [DF_tbl_Buddy_Serial231]  DEFAULT (0xFFFFFFFF) FOR [Serial33]
GO
ALTER TABLE [dbo].[tbl_Buddy] ADD  CONSTRAINT [DF_tbl_Buddy_Serial241]  DEFAULT (0xFFFFFFFF) FOR [Serial34]
GO
ALTER TABLE [dbo].[tbl_Buddy] ADD  CONSTRAINT [DF_tbl_Buddy_Serial251]  DEFAULT (0xFFFFFFFF) FOR [Serial35]
GO
ALTER TABLE [dbo].[tbl_Buddy] ADD  CONSTRAINT [DF_tbl_Buddy_Serial261]  DEFAULT (0xFFFFFFFF) FOR [Serial36]
GO
ALTER TABLE [dbo].[tbl_Buddy] ADD  CONSTRAINT [DF_tbl_Buddy_Serial271]  DEFAULT (0xFFFFFFFF) FOR [Serial37]
GO
ALTER TABLE [dbo].[tbl_Buddy] ADD  CONSTRAINT [DF_tbl_Buddy_Serial281]  DEFAULT (0xFFFFFFFF) FOR [Serial38]
GO
ALTER TABLE [dbo].[tbl_Buddy] ADD  CONSTRAINT [DF_tbl_Buddy_Serial291]  DEFAULT (0xFFFFFFFF) FOR [Serial39]
GO
ALTER TABLE [dbo].[tbl_Buddy] ADD  CONSTRAINT [DF_tbl_Buddy_Serial201_1]  DEFAULT (0xFFFFFFFF) FOR [Serial40]
GO
ALTER TABLE [dbo].[tbl_Buddy] ADD  CONSTRAINT [DF_tbl_Buddy_Serial211_1]  DEFAULT (0xFFFFFFFF) FOR [Serial41]
GO
ALTER TABLE [dbo].[tbl_Buddy] ADD  CONSTRAINT [DF_tbl_Buddy_Serial221_1]  DEFAULT (0xFFFFFFFF) FOR [Serial42]
GO
ALTER TABLE [dbo].[tbl_Buddy] ADD  CONSTRAINT [DF_tbl_Buddy_Serial231_1]  DEFAULT (0xFFFFFFFF) FOR [Serial43]
GO
ALTER TABLE [dbo].[tbl_Buddy] ADD  CONSTRAINT [DF_tbl_Buddy_Serial241_1]  DEFAULT (0xFFFFFFFF) FOR [Serial44]
GO
ALTER TABLE [dbo].[tbl_Buddy] ADD  CONSTRAINT [DF_tbl_Buddy_Serial251_1]  DEFAULT (0xFFFFFFFF) FOR [Serial45]
GO
ALTER TABLE [dbo].[tbl_Buddy] ADD  CONSTRAINT [DF_tbl_Buddy_Serial261_1]  DEFAULT (0xFFFFFFFF) FOR [Serial46]
GO
ALTER TABLE [dbo].[tbl_Buddy] ADD  CONSTRAINT [DF_tbl_Buddy_Serial271_1]  DEFAULT (0xFFFFFFFF) FOR [Serial47]
GO
ALTER TABLE [dbo].[tbl_Buddy] ADD  CONSTRAINT [DF_tbl_Buddy_Serial281_1]  DEFAULT (0xFFFFFFFF) FOR [Serial48]
GO
ALTER TABLE [dbo].[tbl_Buddy] ADD  CONSTRAINT [DF_tbl_Buddy_Serial291_1]  DEFAULT (0xFFFFFFFF) FOR [Serial49]
GO
ALTER TABLE [dbo].[tbl_Buddy] ADD  CONSTRAINT [DF_tbl_Buddy_DCK]  DEFAULT ((0)) FOR [DCK]
GO
ALTER TABLE [dbo].[tbl_Cash_LimSale] ADD  CONSTRAINT [DF_tbl_Cash_LimSale_DCK]  DEFAULT ((0)) FOR [DCK]
GO
ALTER TABLE [dbo].[tbl_Cash_LimSale] ADD  CONSTRAINT [DF_tbl_Cash_LimSale_LimSaleNum]  DEFAULT ((0)) FOR [LimSaleNum]
GO
ALTER TABLE [dbo].[tbl_Cash_LimSale] ADD  CONSTRAINT [DF_tbl_Cash_LimSale_Code_K0]  DEFAULT (0xFFFFFFFF) FOR [Code_K0]
GO
ALTER TABLE [dbo].[tbl_Cash_LimSale] ADD  CONSTRAINT [DF_tbl_Cash_LimSale_Num0]  DEFAULT ((0)) FOR [Num0]
GO
ALTER TABLE [dbo].[tbl_Cash_LimSale] ADD  CONSTRAINT [DF_tbl_Cash_LimSale_Code_K1]  DEFAULT (0xFFFFFFFF) FOR [Code_K1]
GO
ALTER TABLE [dbo].[tbl_Cash_LimSale] ADD  CONSTRAINT [DF_tbl_Cash_LimSale_Num1]  DEFAULT ((0)) FOR [Num1]
GO
ALTER TABLE [dbo].[tbl_Cash_LimSale] ADD  CONSTRAINT [DF_tbl_Cash_LimSale_Code_K2]  DEFAULT (0xFFFFFFFF) FOR [Code_K2]
GO
ALTER TABLE [dbo].[tbl_Cash_LimSale] ADD  CONSTRAINT [DF_tbl_Cash_LimSale_Num2]  DEFAULT ((0)) FOR [Num2]
GO
ALTER TABLE [dbo].[tbl_Cash_LimSale] ADD  CONSTRAINT [DF_tbl_Cash_LimSale_Code_K3]  DEFAULT (0xFFFFFFFF) FOR [Code_K3]
GO
ALTER TABLE [dbo].[tbl_Cash_LimSale] ADD  CONSTRAINT [DF_tbl_Cash_LimSale_Num3]  DEFAULT ((0)) FOR [Num3]
GO
ALTER TABLE [dbo].[tbl_Cash_LimSale] ADD  CONSTRAINT [DF_tbl_Cash_LimSale_Code_K4]  DEFAULT (0xFFFFFFFF) FOR [Code_K4]
GO
ALTER TABLE [dbo].[tbl_Cash_LimSale] ADD  CONSTRAINT [DF_tbl_Cash_LimSale_Num4]  DEFAULT ((0)) FOR [Num4]
GO
ALTER TABLE [dbo].[tbl_Cash_LimSale] ADD  CONSTRAINT [DF_tbl_Cash_LimSale_Code_K5]  DEFAULT (0xFFFFFFFF) FOR [Code_K5]
GO
ALTER TABLE [dbo].[tbl_Cash_LimSale] ADD  CONSTRAINT [DF_tbl_Cash_LimSale_Num5]  DEFAULT ((0)) FOR [Num5]
GO
ALTER TABLE [dbo].[tbl_Cash_LimSale] ADD  CONSTRAINT [DF_tbl_Cash_LimSale_Code_K6]  DEFAULT (0xFFFFFFFF) FOR [Code_K6]
GO
ALTER TABLE [dbo].[tbl_Cash_LimSale] ADD  CONSTRAINT [DF_tbl_Cash_LimSale_Num6]  DEFAULT ((0)) FOR [Num6]
GO
ALTER TABLE [dbo].[tbl_Cash_LimSale] ADD  CONSTRAINT [DF_tbl_Cash_LimSale_Code_K7]  DEFAULT (0xFFFFFFFF) FOR [Code_K7]
GO
ALTER TABLE [dbo].[tbl_Cash_LimSale] ADD  CONSTRAINT [DF_tbl_Cash_LimSale_Num7]  DEFAULT ((0)) FOR [Num7]
GO
ALTER TABLE [dbo].[tbl_Cash_LimSale] ADD  CONSTRAINT [DF_tbl_Cash_LimSale_Code_K8]  DEFAULT (0xFFFFFFFF) FOR [Code_K8]
GO
ALTER TABLE [dbo].[tbl_Cash_LimSale] ADD  CONSTRAINT [DF_tbl_Cash_LimSale_Num8]  DEFAULT ((0)) FOR [Num8]
GO
ALTER TABLE [dbo].[tbl_Cash_LimSale] ADD  CONSTRAINT [DF_tbl_Cash_LimSale_Code_K9]  DEFAULT (0xFFFFFFFF) FOR [Code_K9]
GO
ALTER TABLE [dbo].[tbl_Cash_LimSale] ADD  CONSTRAINT [DF_tbl_Cash_LimSale_Num9]  DEFAULT ((0)) FOR [Num9]
GO
ALTER TABLE [dbo].[tbl_Cash_LimSale] ADD  CONSTRAINT [DF_tbl_Cash_LimSale_Code_K10]  DEFAULT (0xFFFFFFFF) FOR [Code_K10]
GO
ALTER TABLE [dbo].[tbl_Cash_LimSale] ADD  CONSTRAINT [DF_tbl_Cash_LimSale_Num10]  DEFAULT ((0)) FOR [Num10]
GO
ALTER TABLE [dbo].[tbl_Cash_LimSale] ADD  CONSTRAINT [DF_tbl_Cash_LimSale_Code_K11]  DEFAULT (0xFFFFFFFF) FOR [Code_K11]
GO
ALTER TABLE [dbo].[tbl_Cash_LimSale] ADD  CONSTRAINT [DF_tbl_Cash_LimSale_Num11]  DEFAULT ((0)) FOR [Num11]
GO
ALTER TABLE [dbo].[tbl_Cash_LimSale] ADD  CONSTRAINT [DF_tbl_Cash_LimSale_Code_K12]  DEFAULT (0xFFFFFFFF) FOR [Code_K12]
GO
ALTER TABLE [dbo].[tbl_Cash_LimSale] ADD  CONSTRAINT [DF_tbl_Cash_LimSale_Num12]  DEFAULT ((0)) FOR [Num12]
GO
ALTER TABLE [dbo].[tbl_Cash_LimSale] ADD  CONSTRAINT [DF_tbl_Cash_LimSale_Code_K13]  DEFAULT (0xFFFFFFFF) FOR [Code_K13]
GO
ALTER TABLE [dbo].[tbl_Cash_LimSale] ADD  CONSTRAINT [DF_tbl_Cash_LimSale_Num13]  DEFAULT ((0)) FOR [Num13]
GO
ALTER TABLE [dbo].[tbl_Cash_LimSale] ADD  CONSTRAINT [DF_tbl_Cash_LimSale_Code_K14]  DEFAULT (0xFFFFFFFF) FOR [Code_K14]
GO
ALTER TABLE [dbo].[tbl_Cash_LimSale] ADD  CONSTRAINT [DF_tbl_Cash_LimSale_Num14]  DEFAULT ((0)) FOR [Num14]
GO
ALTER TABLE [dbo].[tbl_Cash_LimSale] ADD  CONSTRAINT [DF_tbl_Cash_LimSale_Code_K15]  DEFAULT (0xFFFFFFFF) FOR [Code_K15]
GO
ALTER TABLE [dbo].[tbl_Cash_LimSale] ADD  CONSTRAINT [DF_tbl_Cash_LimSale_Num15]  DEFAULT ((0)) FOR [Num15]
GO
ALTER TABLE [dbo].[tbl_Cash_LimSale] ADD  CONSTRAINT [DF_tbl_Cash_LimSale_Code_K16]  DEFAULT (0xFFFFFFFF) FOR [Code_K16]
GO
ALTER TABLE [dbo].[tbl_Cash_LimSale] ADD  CONSTRAINT [DF_tbl_Cash_LimSale_Num16]  DEFAULT ((0)) FOR [Num16]
GO
ALTER TABLE [dbo].[tbl_Cash_LimSale] ADD  CONSTRAINT [DF_tbl_Cash_LimSale_Code_K17]  DEFAULT (0xFFFFFFFF) FOR [Code_K17]
GO
ALTER TABLE [dbo].[tbl_Cash_LimSale] ADD  CONSTRAINT [DF_tbl_Cash_LimSale_Num17]  DEFAULT ((0)) FOR [Num17]
GO
ALTER TABLE [dbo].[tbl_Cash_LimSale] ADD  CONSTRAINT [DF_tbl_Cash_LimSale_Code_K18]  DEFAULT (0xFFFFFFFF) FOR [Code_K18]
GO
ALTER TABLE [dbo].[tbl_Cash_LimSale] ADD  CONSTRAINT [DF_tbl_Cash_LimSale_Num18]  DEFAULT ((0)) FOR [Num18]
GO
ALTER TABLE [dbo].[tbl_Cash_LimSale] ADD  CONSTRAINT [DF_tbl_Cash_LimSale_Code_K19]  DEFAULT (0xFFFFFFFF) FOR [Code_K19]
GO
ALTER TABLE [dbo].[tbl_Cash_LimSale] ADD  CONSTRAINT [DF_tbl_Cash_LimSale_Num19]  DEFAULT ((0)) FOR [Num19]
GO
ALTER TABLE [dbo].[tbl_characterselect_log_202410] ADD  CONSTRAINT [DF_tbl_characterselect_log_202410_AccountSerial]  DEFAULT ((0)) FOR [AccountSerial]
GO
ALTER TABLE [dbo].[tbl_characterselect_log_202410] ADD  CONSTRAINT [DF_tbl_characterselect_log_202410_Account]  DEFAULT ('*') FOR [Account]
GO
ALTER TABLE [dbo].[tbl_characterselect_log_202410] ADD  CONSTRAINT [DF_tbl_characterselect_log_202410_CharacName]  DEFAULT ('*') FOR [CharacName]
GO
ALTER TABLE [dbo].[tbl_characterselect_log_202410] ADD  CONSTRAINT [DF_tbl_characterselect_log_202410_LogDate]  DEFAULT ((0)) FOR [LogDate]
GO
ALTER TABLE [dbo].[tbl_characterselect_log_202601] ADD  CONSTRAINT [DF_tbl_characterselect_log_202601_AccountSerial]  DEFAULT ((0)) FOR [AccountSerial]
GO
ALTER TABLE [dbo].[tbl_characterselect_log_202601] ADD  CONSTRAINT [DF_tbl_characterselect_log_202601_Account]  DEFAULT ('*') FOR [Account]
GO
ALTER TABLE [dbo].[tbl_characterselect_log_202601] ADD  CONSTRAINT [DF_tbl_characterselect_log_202601_CharacName]  DEFAULT ('*') FOR [CharacName]
GO
ALTER TABLE [dbo].[tbl_characterselect_log_202601] ADD  CONSTRAINT [DF_tbl_characterselect_log_202601_LogDate]  DEFAULT ((0)) FOR [LogDate]
GO
ALTER TABLE [dbo].[tbl_CryMsg] ADD  CONSTRAINT [DF_tbl_CryMsg_CryMsg1]  DEFAULT ('') FOR [CryMsg1]
GO
ALTER TABLE [dbo].[tbl_CryMsg] ADD  CONSTRAINT [DF_tbl_CryMsg_CryMsg2]  DEFAULT ('') FOR [CryMsg2]
GO
ALTER TABLE [dbo].[tbl_CryMsg] ADD  CONSTRAINT [DF_tbl_CryMsg_CryMsg3]  DEFAULT ('') FOR [CryMsg3]
GO
ALTER TABLE [dbo].[tbl_CryMsg] ADD  CONSTRAINT [DF_tbl_CryMsg_CryMsg4]  DEFAULT ('') FOR [CryMsg4]
GO
ALTER TABLE [dbo].[tbl_CryMsg] ADD  CONSTRAINT [DF_tbl_CryMsg_CryMsg5]  DEFAULT ('') FOR [CryMsg5]
GO
ALTER TABLE [dbo].[tbl_CryMsg] ADD  CONSTRAINT [DF_tbl_CryMsg_CryMsg6]  DEFAULT ('') FOR [CryMsg6]
GO
ALTER TABLE [dbo].[tbl_CryMsg] ADD  CONSTRAINT [DF_tbl_CryMsg_CryMsg7]  DEFAULT ('') FOR [CryMsg7]
GO
ALTER TABLE [dbo].[tbl_CryMsg] ADD  CONSTRAINT [DF_tbl_CryMsg_CryMsg8]  DEFAULT ('') FOR [CryMsg8]
GO
ALTER TABLE [dbo].[tbl_CryMsg] ADD  CONSTRAINT [DF_tbl_CryMsg_CryMsg9]  DEFAULT ('') FOR [CryMsg9]
GO
ALTER TABLE [dbo].[tbl_CryMsg] ADD  CONSTRAINT [DF_tbl_CryMsg_CryMsg10]  DEFAULT ('') FOR [CryMsg10]
GO
ALTER TABLE [dbo].[tbl_economy_history] ADD  CONSTRAINT [DF_tbl_economy_history_Date]  DEFAULT (getdate()) FOR [Updatedate]
GO
ALTER TABLE [dbo].[tbl_economy_history] ADD  CONSTRAINT [DF_tbl_economy_history_B_Dalant]  DEFAULT ((1)) FOR [B_Dalant]
GO
ALTER TABLE [dbo].[tbl_economy_history] ADD  CONSTRAINT [DF_tbl_economy_history_B_Gold]  DEFAULT ((1)) FOR [B_Gold]
GO
ALTER TABLE [dbo].[tbl_economy_history] ADD  CONSTRAINT [DF_tbl_economy_history_C_Dalant]  DEFAULT ((1)) FOR [C_Dalant]
GO
ALTER TABLE [dbo].[tbl_economy_history] ADD  CONSTRAINT [DF_tbl_economy_history_C_Gold]  DEFAULT ((1)) FOR [C_Gold]
GO
ALTER TABLE [dbo].[tbl_economy_history] ADD  CONSTRAINT [DF_tbl_economy_history_A_Dalant]  DEFAULT ((1)) FOR [A_Dalant]
GO
ALTER TABLE [dbo].[tbl_economy_history] ADD  CONSTRAINT [DF_tbl_economy_history_A_Gold]  DEFAULT ((1)) FOR [A_Gold]
GO
ALTER TABLE [dbo].[tbl_economy_history] ADD  CONSTRAINT [DF_tbl_economy_history_ManageValue]  DEFAULT ((1000)) FOR [ManageValue]
GO
ALTER TABLE [dbo].[tbl_economy_history] ADD  CONSTRAINT [DF_tbl_economy_history_B_MineOre1]  DEFAULT ((0)) FOR [B_MineOre1]
GO
ALTER TABLE [dbo].[tbl_economy_history] ADD  CONSTRAINT [DF_tbl_economy_history_B_MineOre2]  DEFAULT ((0)) FOR [B_MineOre2]
GO
ALTER TABLE [dbo].[tbl_economy_history] ADD  CONSTRAINT [DF_tbl_economy_history_B_MineOre3]  DEFAULT ((0)) FOR [B_MineOre3]
GO
ALTER TABLE [dbo].[tbl_economy_history] ADD  CONSTRAINT [DF_tbl_economy_history_C_MineOre1]  DEFAULT ((0)) FOR [C_MineOre1]
GO
ALTER TABLE [dbo].[tbl_economy_history] ADD  CONSTRAINT [DF_tbl_economy_history_C_MineOre2]  DEFAULT ((0)) FOR [C_MineOre2]
GO
ALTER TABLE [dbo].[tbl_economy_history] ADD  CONSTRAINT [DF_tbl_economy_history_C_MineOre3]  DEFAULT ((0)) FOR [C_MineOre3]
GO
ALTER TABLE [dbo].[tbl_economy_history] ADD  CONSTRAINT [DF_tbl_economy_history_A_MineOre2]  DEFAULT ((0)) FOR [A_MineOre2]
GO
ALTER TABLE [dbo].[tbl_economy_history] ADD  CONSTRAINT [DF_tbl_economy_history_A_MineOre3]  DEFAULT ((0)) FOR [A_MineOre3]
GO
ALTER TABLE [dbo].[tbl_economy_history] ADD  CONSTRAINT [DF_tbl_economy_history_B_CutOre1_1]  DEFAULT ((0)) FOR [B_CutOre1]
GO
ALTER TABLE [dbo].[tbl_economy_history] ADD  CONSTRAINT [DF_tbl_economy_history_B_CutOre2_1]  DEFAULT ((0)) FOR [B_CutOre2]
GO
ALTER TABLE [dbo].[tbl_economy_history] ADD  CONSTRAINT [DF_tbl_economy_history_B_CutOre3_1]  DEFAULT ((0)) FOR [B_CutOre3]
GO
ALTER TABLE [dbo].[tbl_economy_history] ADD  CONSTRAINT [DF_tbl_economy_history_C_CutOre1]  DEFAULT ((0)) FOR [C_CutOre1]
GO
ALTER TABLE [dbo].[tbl_economy_history] ADD  CONSTRAINT [DF_tbl_economy_history_C_CutOre2]  DEFAULT ((0)) FOR [C_CutOre2]
GO
ALTER TABLE [dbo].[tbl_economy_history] ADD  CONSTRAINT [DF_tbl_economy_history_C_CutOre3]  DEFAULT ((0)) FOR [C_CutOre3]
GO
ALTER TABLE [dbo].[tbl_economy_history] ADD  CONSTRAINT [DF_tbl_economy_history_A_CutOre1_1]  DEFAULT ((0)) FOR [A_CutOre1]
GO
ALTER TABLE [dbo].[tbl_economy_history] ADD  CONSTRAINT [DF_tbl_economy_history_A_CutOre2_1]  DEFAULT ((0)) FOR [A_CutOre2]
GO
ALTER TABLE [dbo].[tbl_economy_history] ADD  CONSTRAINT [DF_tbl_economy_history_A_CutOre3_1]  DEFAULT ((0)) FOR [A_CutOre3]
GO
ALTER TABLE [dbo].[tbl_event] ADD  CONSTRAINT [DF_tbl_event_ClassRefineCnt]  DEFAULT ((0)) FOR [ClassRefineCnt]
GO
ALTER TABLE [dbo].[tbl_event] ADD  CONSTRAINT [DF__tbl_event_ClassRefineDate]  DEFAULT ((0)) FOR [ClassRefineDate]
GO
ALTER TABLE [dbo].[tbl_EventPoint] ADD  CONSTRAINT [DF_tbl_EventPoint_DCK]  DEFAULT ((0)) FOR [DCK]
GO
ALTER TABLE [dbo].[tbl_EventPoint] ADD  CONSTRAINT [DF_tbl_EventPoint_Point]  DEFAULT ((0)) FOR [Point]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF_tbl_general_Serial]  DEFAULT ((-1)) FOR [Serial]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF_tbl_general_DCK]  DEFAULT ((0)) FOR [DCK]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF_tbl_general_TotalPlayMin]  DEFAULT ((0)) FOR [TotalPlayMin]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF_tbl_general_Exp]  DEFAULT ((0)) FOR [Exp]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF_tbl_general_LossExp]  DEFAULT ((0)) FOR [LossExp]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF_tbl_general_HP]  DEFAULT ((1000)) FOR [HP]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF_tbl_general_FP]  DEFAULT ((1000)) FOR [FP]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF_tbl_general_SP]  DEFAULT ((1000)) FOR [SP]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF_tbl_general_DP]  DEFAULT ((0)) FOR [DP]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF_tbl_general_Map]  DEFAULT ((0)) FOR [Map]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF_tbl_general_X]  DEFAULT ((0)) FOR [X]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF_tbl_general_Y]  DEFAULT ((65535)) FOR [Y]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF_tbl_general_Z]  DEFAULT ((0)) FOR [Z]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF_tbl_general_BagNum]  DEFAULT ((1)) FOR [BagNum]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF_tbl_general_EK0]  DEFAULT (0xFFFFFFFF) FOR [EK0]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF_tbl_general_ED0]  DEFAULT ((0)) FOR [ED0]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF_tbl_general_EK1]  DEFAULT (0xFFFFFFFF) FOR [EK1]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF_tbl_general_ED1]  DEFAULT ((0)) FOR [ED1]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF_tbl_general_EK2]  DEFAULT (0xFFFFFFFF) FOR [EK2]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF_tbl_general_ED2]  DEFAULT ((0)) FOR [ED2]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF_tbl_general_EK3]  DEFAULT (0xFFFFFFFF) FOR [EK3]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF_tbl_general_ED3]  DEFAULT ((0)) FOR [ED3]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF_tbl_general_EK4]  DEFAULT (0xFFFFFFFF) FOR [EK4]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF_tbl_general_ED4]  DEFAULT ((0)) FOR [ED4]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF_tbl_general_EK5]  DEFAULT (0xFFFFFFFF) FOR [EK5]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF_tbl_general_ED5]  DEFAULT ((0)) FOR [ED5]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF_tbl_general_F0]  DEFAULT (0xFFFFFFFF) FOR [F0]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF_tbl_general_F1]  DEFAULT (0xFFFFFFFF) FOR [F1]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF_tbl_general_F2]  DEFAULT (0xFFFFFFFF) FOR [F2]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF_tbl_general_F3]  DEFAULT (0xFFFFFFFF) FOR [F3]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF_tbl_general_F4]  DEFAULT (0xFFFFFFFF) FOR [F4]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF_tbl_general_F5]  DEFAULT (0xFFFFFFFF) FOR [F5]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF_tbl_general_F6]  DEFAULT (0xFFFFFFFF) FOR [F6]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF_tbl_general_F7]  DEFAULT (0xFFFFFFFF) FOR [F7]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF_tbl_general_F8]  DEFAULT (0xFFFFFFFF) FOR [F8]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF_tbl_general_F9]  DEFAULT (0xFFFFFFFF) FOR [F9]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF_tbl_general_F10]  DEFAULT (0xFFFFFFFF) FOR [F10]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF_tbl_general_F11]  DEFAULT (0xFFFFFFFF) FOR [F11]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF_tbl_general_F12]  DEFAULT (0xFFFFFFFF) FOR [F12]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF_tbl_general_F13]  DEFAULT (0xFFFFFFFF) FOR [F13]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF_tbl_general_F14]  DEFAULT (0xFFFFFFFF) FOR [F14]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF_tbl_general_F15]  DEFAULT (0xFFFFFFFF) FOR [F15]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF_tbl_general_F16]  DEFAULT (0xFFFFFFFF) FOR [F16]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF_tbl_general_F17]  DEFAULT (0xFFFFFFFF) FOR [F17]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF_tbl_general_F18]  DEFAULT (0xFFFFFFFF) FOR [F18]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF_tbl_general_F19]  DEFAULT (0xFFFFFFFF) FOR [F19]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF_tbl_general_F20]  DEFAULT (0xFFFFFFFF) FOR [F20]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF_tbl_general_F21]  DEFAULT (0xFFFFFFFF) FOR [F21]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF_tbl_general_F22]  DEFAULT (0xFFFFFFFF) FOR [F22]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF_tbl_general_F23]  DEFAULT (0xFFFFFFFF) FOR [F23]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF_tbl_general_F24]  DEFAULT (0xFFFFFFFF) FOR [F24]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF_tbl_general_F25]  DEFAULT (0xFFFFFFFF) FOR [F25]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF_tbl_general_F26]  DEFAULT (0xFFFFFFFF) FOR [F26]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF_tbl_general_F27]  DEFAULT (0xFFFFFFFF) FOR [F27]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF_tbl_general_F28]  DEFAULT (0xFFFFFFFF) FOR [F28]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF_tbl_general_F29]  DEFAULT (0xFFFFFFFF) FOR [F29]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF_tbl_general_F30]  DEFAULT (0xFFFFFFFF) FOR [F30]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF_tbl_general_F31]  DEFAULT (0xFFFFFFFF) FOR [F31]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF_tbl_general_F32]  DEFAULT (0xFFFFFFFF) FOR [F32]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF_tbl_general_F33]  DEFAULT (0xFFFFFFFF) FOR [F33]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF_tbl_general_F34]  DEFAULT (0xFFFFFFFF) FOR [F34]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF_tbl_general_F35]  DEFAULT (0xFFFFFFFF) FOR [F35]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF_tbl_general_F36]  DEFAULT (0xFFFFFFFF) FOR [F36]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF_tbl_general_F37]  DEFAULT (0xFFFFFFFF) FOR [F37]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF_tbl_general_F38]  DEFAULT (0xFFFFFFFF) FOR [F38]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF_tbl_general_F39]  DEFAULT (0xFFFFFFFF) FOR [F39]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF_tbl_general_F40]  DEFAULT (0xFFFFFFFF) FOR [F40]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF_tbl_general_F41]  DEFAULT (0xFFFFFFFF) FOR [F41]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF_tbl_general_F42]  DEFAULT (0xFFFFFFFF) FOR [F42]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF_tbl_general_F43]  DEFAULT (0xFFFFFFFF) FOR [F43]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF_tbl_general_F44]  DEFAULT (0xFFFFFFFF) FOR [F44]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF_tbl_general_F45]  DEFAULT (0xFFFFFFFF) FOR [F45]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF_tbl_general_F46]  DEFAULT (0xFFFFFFFF) FOR [F46]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF_tbl_general_F47]  DEFAULT (0xFFFFFFFF) FOR [F47]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF_tbl_general_F48]  DEFAULT (0xFFFFFFFF) FOR [F48]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF_tbl_general_F49]  DEFAULT (0xFFFFFFFF) FOR [F49]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF_tbl_general_F50]  DEFAULT (0xFFFFFFFF) FOR [F50]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF_tbl_general_F51]  DEFAULT (0xFFFFFFFF) FOR [F51]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF_tbl_general_F52]  DEFAULT (0xFFFFFFFF) FOR [F52]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF_tbl_general_F53]  DEFAULT (0xFFFFFFFF) FOR [F53]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF_tbl_general_F54]  DEFAULT (0xFFFFFFFF) FOR [F54]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF_tbl_general_F55]  DEFAULT (0xFFFFFFFF) FOR [F55]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF_tbl_general_F56]  DEFAULT (0xFFFFFFFF) FOR [F56]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF_tbl_general_F57]  DEFAULT (0xFFFFFFFF) FOR [F57]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF_tbl_general_F58]  DEFAULT (0xFFFFFFFF) FOR [F58]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF_tbl_general_F59]  DEFAULT (0xFFFFFFFF) FOR [F59]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF_tbl_general_F60]  DEFAULT (0xFFFFFFFF) FOR [F60]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF_tbl_general_F61]  DEFAULT (0xFFFFFFFF) FOR [F61]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF_tbl_general_F62]  DEFAULT (0xFFFFFFFF) FOR [F62]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF_tbl_general_F63]  DEFAULT (0xFFFFFFFF) FOR [F63]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF_tbl_general_F64]  DEFAULT (0xFFFFFFFF) FOR [F64]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF_tbl_general_F65]  DEFAULT (0xFFFFFFFF) FOR [F65]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF_tbl_general_F66]  DEFAULT (0xFFFFFFFF) FOR [F66]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF_tbl_general_F67]  DEFAULT (0xFFFFFFFF) FOR [F67]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF_tbl_general_F68]  DEFAULT (0xFFFFFFFF) FOR [F68]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF_tbl_general_F69]  DEFAULT (0xFFFFFFFF) FOR [F69]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF_tbl_general_F70]  DEFAULT (0xFFFFFFFF) FOR [F70]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF_tbl_general_F71]  DEFAULT (0xFFFFFFFF) FOR [F71]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF_tbl_general_F72]  DEFAULT (0xFFFFFFFF) FOR [F72]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF_tbl_general_F73]  DEFAULT (0xFFFFFFFF) FOR [F73]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF_tbl_general_F74]  DEFAULT (0xFFFFFFFF) FOR [F74]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF_tbl_general_F75]  DEFAULT (0xFFFFFFFF) FOR [F75]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF_tbl_general_F76]  DEFAULT (0xFFFFFFFF) FOR [F76]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF_tbl_general_F77]  DEFAULT (0xFFFFFFFF) FOR [F77]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF_tbl_general_F78]  DEFAULT (0xFFFFFFFF) FOR [F78]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF_tbl_general_F79]  DEFAULT (0xFFFFFFFF) FOR [F79]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF_tbl_general_F80]  DEFAULT (0xFFFFFFFF) FOR [F80]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF_tbl_general_F81]  DEFAULT (0xFFFFFFFF) FOR [F81]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF_tbl_general_F82]  DEFAULT (0xFFFFFFFF) FOR [F82]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF_tbl_general_F83]  DEFAULT (0xFFFFFFFF) FOR [F83]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF_tbl_general_F84]  DEFAULT (0xFFFFFFFF) FOR [F84]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF_tbl_general_F85]  DEFAULT (0xFFFFFFFF) FOR [F85]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF_tbl_general_F86]  DEFAULT (0xFFFFFFFF) FOR [F86]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF_tbl_general_F87]  DEFAULT (0xFFFFFFFF) FOR [F87]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF_tbl_general_F88]  DEFAULT (0xFFFFFFFF) FOR [F88]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF_tbl_general_F89]  DEFAULT (0xFFFFFFFF) FOR [F89]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF_tbl_general_F90]  DEFAULT (0xFFFFFFFF) FOR [F90]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF_tbl_general_F91]  DEFAULT (0xFFFFFFFF) FOR [F91]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF_tbl_general_F92]  DEFAULT (0xFFFFFFFF) FOR [F92]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF_tbl_general_F93]  DEFAULT (0xFFFFFFFF) FOR [F93]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF_tbl_general_F94]  DEFAULT (0xFFFFFFFF) FOR [F94]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF_tbl_general_F95]  DEFAULT (0xFFFFFFFF) FOR [F95]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF_tbl_general_F96]  DEFAULT (0xFFFFFFFF) FOR [F96]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF_tbl_general_F97]  DEFAULT (0xFFFFFFFF) FOR [F97]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF_tbl_general_F98]  DEFAULT (0xFFFFFFFF) FOR [F98]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF_tbl_general_F99]  DEFAULT (0xFFFFFFFF) FOR [F99]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF_tbl_general_AK0]  DEFAULT (0xFF) FOR [AK0]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF_tbl_general_AD0]  DEFAULT ((0)) FOR [AD0]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF_tbl_general_AP]  DEFAULT ((0)) FOR [AP0]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF_tbl_general_AK01]  DEFAULT (0xFF) FOR [AK1]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF_tbl_general_AD1]  DEFAULT ((0)) FOR [AD1]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF_tbl_general_AP1]  DEFAULT ((0)) FOR [AP1]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF_tbl_general_AK01_1]  DEFAULT (0xFF) FOR [AK2]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF_tbl_general_AD2]  DEFAULT ((0)) FOR [AD2]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF_tbl_general_AP2]  DEFAULT ((0)) FOR [AP2]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF_tbl_general_AK01_2]  DEFAULT (0xFF) FOR [AK3]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF_tbl_general_AD3]  DEFAULT ((0)) FOR [AD3]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF_tbl_general_AP3]  DEFAULT ((0)) FOR [AP3]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF_tbl_general_WM0]  DEFAULT ((0)) FOR [WM0]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF_tbl_general_WM1]  DEFAULT ((0)) FOR [WM1]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF_tbl_general_DM]  DEFAULT ((0)) FOR [DM]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF_tbl_general_PM]  DEFAULT ((0)) FOR [PM]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF_tbl_general_MI]  DEFAULT ((0)) FOR [MI0]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF_tbl_general_MI1]  DEFAULT ((0)) FOR [MI1]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF_tbl_general_MI2]  DEFAULT ((0)) FOR [MI2]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF_tbl_general_SR]  DEFAULT ((0)) FOR [SR]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF_tbl_general_FM0]  DEFAULT ((0)) FOR [FM0]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF_tbl_general_FM1]  DEFAULT ((0)) FOR [FM1]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF_tbl_general_FM2]  DEFAULT ((0)) FOR [FM2]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF_tbl_general_FM3]  DEFAULT ((0)) FOR [FM3]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF_tbl_general_FM4]  DEFAULT ((0)) FOR [FM4]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF_tbl_general_FM5]  DEFAULT ((0)) FOR [FM5]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF_tbl_general_FM6]  DEFAULT ((0)) FOR [FM6]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF_tbl_general_FM7]  DEFAULT ((0)) FOR [FM7]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF_tbl_general_FM8]  DEFAULT ((0)) FOR [FM8]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF_tbl_general_FM9]  DEFAULT ((0)) FOR [FM9]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF_tbl_general_FM10]  DEFAULT ((0)) FOR [FM10]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF_tbl_general_FM11]  DEFAULT ((0)) FOR [FM11]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF_tbl_general_FM12]  DEFAULT ((0)) FOR [FM12]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF_tbl_general_FM13]  DEFAULT ((0)) FOR [FM13]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF_tbl_general_FM14]  DEFAULT ((0)) FOR [FM14]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF_tbl_general_FM15]  DEFAULT ((0)) FOR [FM15]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF_tbl_general_FM16]  DEFAULT ((0)) FOR [FM16]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF_tbl_general_FM17]  DEFAULT ((0)) FOR [FM17]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF_tbl_general_FM18]  DEFAULT ((0)) FOR [FM18]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF_tbl_general_FM19]  DEFAULT ((0)) FOR [FM19]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF_tbl_general_FM20]  DEFAULT ((0)) FOR [FM20]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF_tbl_general_FM21]  DEFAULT ((0)) FOR [FM21]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF_tbl_general_FM22]  DEFAULT ((0)) FOR [FM22]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF_tbl_general_FM23]  DEFAULT ((0)) FOR [FM23]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF_tbl_general_SM0]  DEFAULT ((0)) FOR [SM0]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF_tbl_general_SM1]  DEFAULT ((0)) FOR [SM1]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF_tbl_general_SM2]  DEFAULT ((0)) FOR [SM2]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF_tbl_general_SM3]  DEFAULT ((0)) FOR [SM3]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF_tbl_general_SM4]  DEFAULT ((0)) FOR [SM4]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF_tbl_general_SM5]  DEFAULT ((0)) FOR [SM5]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF_tbl_general_SM6]  DEFAULT ((0)) FOR [SM6]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF_tbl_general_SM7]  DEFAULT ((0)) FOR [SM7]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF_tbl_general_SM8]  DEFAULT ((0)) FOR [SM8]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF_tbl_general_SM9]  DEFAULT ((0)) FOR [SM9]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF_tbl_general_SM10]  DEFAULT ((0)) FOR [SM10]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF_tbl_general_SM11]  DEFAULT ((0)) FOR [SM11]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF_tbl_general_SM12]  DEFAULT ((0)) FOR [SM12]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF_tbl_general_SM13]  DEFAULT ((0)) FOR [SM13]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF_tbl_general_SM14]  DEFAULT ((0)) FOR [SM14]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF_tbl_general_SM15]  DEFAULT ((0)) FOR [SM15]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF_tbl_general_SM16]  DEFAULT ((0)) FOR [SM16]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF_tbl_general_SM17]  DEFAULT ((0)) FOR [SM17]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF_tbl_general_SM18]  DEFAULT ((0)) FOR [SM18]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF_tbl_general_SM19]  DEFAULT ((0)) FOR [SM19]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF_tbl_general_SM20]  DEFAULT ((0)) FOR [SM20]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF_tbl_general_SM21]  DEFAULT ((0)) FOR [SM21]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF_tbl_general_SM22]  DEFAULT ((0)) FOR [SM22]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF_tbl_general_SM23]  DEFAULT ((0)) FOR [SM23]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF_tbl_general_SM01]  DEFAULT ((0)) FOR [SM24]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF_tbl_general_SM02]  DEFAULT ((0)) FOR [SM25]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF_tbl_general_SM03]  DEFAULT ((0)) FOR [SM26]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF_tbl_general_SM04]  DEFAULT ((0)) FOR [SM27]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF_tbl_general_SM05]  DEFAULT ((0)) FOR [SM28]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF_tbl_general_SM06]  DEFAULT ((0)) FOR [SM29]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF_tbl_general_SM01_1]  DEFAULT ((0)) FOR [SM30]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF_tbl_general_SM211]  DEFAULT ((0)) FOR [SM31]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF_tbl_general_SM221]  DEFAULT ((0)) FOR [SM32]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF_tbl_general_SM231]  DEFAULT ((0)) FOR [SM33]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF_tbl_general_SM241]  DEFAULT ((0)) FOR [SM34]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF_tbl_general_SM251]  DEFAULT ((0)) FOR [SM35]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF_tbl_general_SM261]  DEFAULT ((0)) FOR [SM36]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF_tbl_general_SM271]  DEFAULT ((0)) FOR [SM37]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF_tbl_general_SM281]  DEFAULT ((0)) FOR [SM38]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF_tbl_general_SM291]  DEFAULT ((0)) FOR [SM39]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF_tbl_general_SM40]  DEFAULT ((0)) FOR [SM40]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF_tbl_general_SM401]  DEFAULT ((0)) FOR [SM41]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF_tbl_general_SM402]  DEFAULT ((0)) FOR [SM42]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF_tbl_general_SM403]  DEFAULT ((0)) FOR [SM43]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF_tbl_general_SM404]  DEFAULT ((0)) FOR [SM44]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF_tbl_general_SM405]  DEFAULT ((0)) FOR [SM45]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF_tbl_general_SM406]  DEFAULT ((0)) FOR [SM46]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF_tbl_general_SM407]  DEFAULT ((0)) FOR [SM47]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF_tbl_general_SM408]  DEFAULT ((0)) FOR [SM48]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF_tbl_general_SM409]  DEFAULT ((0)) FOR [SM49]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF_tbl_general_LeftResList]  DEFAULT ('*') FOR [LeftResList]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF_tbl_general_Class0]  DEFAULT ((-1)) FOR [Class0]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF_tbl_general_Class1]  DEFAULT ((-1)) FOR [Class1]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF_tbl_general_Class2]  DEFAULT ((-1)) FOR [Class2]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF_tbl_general_ClassInitCnt]  DEFAULT ((0)) FOR [ClassInitCnt]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF_tbl_general_LastClassGrade]  DEFAULT ((0)) FOR [LastClassGrade]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF_tbl_general_PvPPoint]  DEFAULT ((0)) FOR [PvpPoint]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF_tbl_general_PvPCashBag]  DEFAULT ((0)) FOR [PvpCashBag]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF_tbl_general_BindMapCode]  DEFAULT ('0') FOR [BindMapCode]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF_tbl_general_BindDummy]  DEFAULT ('0') FOR [BindDummy]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF_tbl_General_GuildSerial]  DEFAULT (0xFFFFFFFF) FOR [GuildSerial]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF_tbl_General_GuildGrade]  DEFAULT (0xFF) FOR [GuildGrade]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF_tbl_general_GuildRank]  DEFAULT (0xFFFF) FOR [GuildRank]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF_tbl_general_PvP_0]  DEFAULT ((0)) FOR [PvP_0]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF_tbl_general_PK_0]  DEFAULT ((0)) FOR [PK_0]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF_tbl_general_PvP_1]  DEFAULT ((0)) FOR [PvP_1]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF_tbl_general_PK_1]  DEFAULT ((0)) FOR [PK_1]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF_tbl_general_PvP_2]  DEFAULT ((0)) FOR [PvP_2]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF_tbl_general_PK_2]  DEFAULT ((0)) FOR [PK_2]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF_tbl_general__CharacterGrade]  DEFAULT ((0)) FOR [CharacterGrade]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF_tbl_general_Die_0]  DEFAULT ((0)) FOR [Die_0]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF_tbl_general_Die_1]  DEFAULT ((0)) FOR [Die_1]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF_tbl_general_Die_2]  DEFAULT ((0)) FOR [Die_2]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF_tbl_general_tmracebossvote]  DEFAULT ('1990-01-01') FOR [tmRaceBossVote]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF__tbl_gener__TakeL__2D73677A]  DEFAULT ((0)) FOR [TakeLastMentalTicket]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF__tbl_gener__TakeL__304FD425]  DEFAULT ((0)) FOR [TakeLastCriTicket]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF__tbl_gener__RDDel__332C40D0]  DEFAULT ((0)) FOR [RDDelay]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF__tbl_general__ES0__7442102D]  DEFAULT ((0)) FOR [ES0]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF__tbl_general__ET0__75363466]  DEFAULT (0xFFFFFFFF) FOR [ET0]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF__tbl_general__ES1__762A589F]  DEFAULT ((0)) FOR [ES1]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF__tbl_general__ET1__771E7CD8]  DEFAULT (0xFFFFFFFF) FOR [ET1]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF__tbl_general__ES2__7812A111]  DEFAULT ((0)) FOR [ES2]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF__tbl_general__ET2__7906C54A]  DEFAULT (0xFFFFFFFF) FOR [ET2]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF__tbl_general__ES3__79FAE983]  DEFAULT ((0)) FOR [ES3]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF__tbl_general__ET3__7AEF0DBC]  DEFAULT (0xFFFFFFFF) FOR [ET3]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF__tbl_general__ES4__7BE331F5]  DEFAULT ((0)) FOR [ES4]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF__tbl_general__ET4__7CD7562E]  DEFAULT (0xFFFFFFFF) FOR [ET4]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF__tbl_general__ES5__7DCB7A67]  DEFAULT ((0)) FOR [ES5]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF__tbl_general__ET5__7EBF9EA0]  DEFAULT (0xFFFFFFFF) FOR [ET5]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF__tbl_general__AS0__7FB3C2D9]  DEFAULT ((0)) FOR [AS0]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF__tbl_general__AS1__00A7E712]  DEFAULT ((0)) FOR [AS1]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF__tbl_general__AS2__019C0B4B]  DEFAULT ((0)) FOR [AS2]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF__tbl_general__AS3__02902F84]  DEFAULT ((0)) FOR [AS3]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF__tbl_general__FS0__038453BD]  DEFAULT ((0)) FOR [FS0]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF__tbl_general__FS1__047877F6]  DEFAULT ((0)) FOR [FS1]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF__tbl_general__FS2__056C9C2F]  DEFAULT ((0)) FOR [FS2]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF__tbl_general__FS3__0660C068]  DEFAULT ((0)) FOR [FS3]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF__tbl_general__FS4__0754E4A1]  DEFAULT ((0)) FOR [FS4]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF__tbl_general__FS5__084908DA]  DEFAULT ((0)) FOR [FS5]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF__tbl_general__FS6__093D2D13]  DEFAULT ((0)) FOR [FS6]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF__tbl_general__FS7__0A31514C]  DEFAULT ((0)) FOR [FS7]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF__tbl_general__FS8__0B257585]  DEFAULT ((0)) FOR [FS8]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF__tbl_general__FS9__0C1999BE]  DEFAULT ((0)) FOR [FS9]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF__tbl_genera__FS10__0D0DBDF7]  DEFAULT ((0)) FOR [FS10]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF__tbl_genera__FS11__0E01E230]  DEFAULT ((0)) FOR [FS11]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF__tbl_genera__FS12__0EF60669]  DEFAULT ((0)) FOR [FS12]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF__tbl_genera__FS13__0FEA2AA2]  DEFAULT ((0)) FOR [FS13]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF__tbl_genera__FS14__10DE4EDB]  DEFAULT ((0)) FOR [FS14]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF__tbl_genera__FS15__11D27314]  DEFAULT ((0)) FOR [FS15]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF__tbl_genera__FS16__12C6974D]  DEFAULT ((0)) FOR [FS16]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF__tbl_genera__FS17__13BABB86]  DEFAULT ((0)) FOR [FS17]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF__tbl_genera__FS18__14AEDFBF]  DEFAULT ((0)) FOR [FS18]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF__tbl_genera__FS19__15A303F8]  DEFAULT ((0)) FOR [FS19]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF__tbl_genera__FS20__16972831]  DEFAULT ((0)) FOR [FS20]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF__tbl_genera__FS21__178B4C6A]  DEFAULT ((0)) FOR [FS21]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF__tbl_genera__FS22__187F70A3]  DEFAULT ((0)) FOR [FS22]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF__tbl_genera__FS23__197394DC]  DEFAULT ((0)) FOR [FS23]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF__tbl_genera__FS24__1A67B915]  DEFAULT ((0)) FOR [FS24]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF__tbl_genera__FS25__1B5BDD4E]  DEFAULT ((0)) FOR [FS25]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF__tbl_genera__FS26__1C500187]  DEFAULT ((0)) FOR [FS26]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF__tbl_genera__FS27__1D4425C0]  DEFAULT ((0)) FOR [FS27]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF__tbl_genera__FS28__1E3849F9]  DEFAULT ((0)) FOR [FS28]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF__tbl_genera__FS29__1F2C6E32]  DEFAULT ((0)) FOR [FS29]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF__tbl_genera__FS30__2020926B]  DEFAULT ((0)) FOR [FS30]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF__tbl_genera__FS31__2114B6A4]  DEFAULT ((0)) FOR [FS31]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF__tbl_genera__FS32__2208DADD]  DEFAULT ((0)) FOR [FS32]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF__tbl_genera__FS33__22FCFF16]  DEFAULT ((0)) FOR [FS33]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF__tbl_genera__FS34__23F1234F]  DEFAULT ((0)) FOR [FS34]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF__tbl_genera__FS35__24E54788]  DEFAULT ((0)) FOR [FS35]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF__tbl_genera__FS36__25D96BC1]  DEFAULT ((0)) FOR [FS36]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF__tbl_genera__FS37__26CD8FFA]  DEFAULT ((0)) FOR [FS37]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF__tbl_genera__FS38__27C1B433]  DEFAULT ((0)) FOR [FS38]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF__tbl_genera__FS39__28B5D86C]  DEFAULT ((0)) FOR [FS39]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF__tbl_genera__FS40__29A9FCA5]  DEFAULT ((0)) FOR [FS40]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF__tbl_genera__FS41__2A9E20DE]  DEFAULT ((0)) FOR [FS41]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF__tbl_genera__FS42__2B924517]  DEFAULT ((0)) FOR [FS42]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF__tbl_genera__FS43__2C866950]  DEFAULT ((0)) FOR [FS43]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF__tbl_genera__FS44__2D7A8D89]  DEFAULT ((0)) FOR [FS44]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF__tbl_genera__FS45__2E6EB1C2]  DEFAULT ((0)) FOR [FS45]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF__tbl_genera__FS46__2F62D5FB]  DEFAULT ((0)) FOR [FS46]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF__tbl_genera__FS47__3056FA34]  DEFAULT ((0)) FOR [FS47]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF__tbl_genera__FS48__314B1E6D]  DEFAULT ((0)) FOR [FS48]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF__tbl_genera__FS49__323F42A6]  DEFAULT ((0)) FOR [FS49]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF__tbl_genera__FS50__333366DF]  DEFAULT ((0)) FOR [FS50]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF__tbl_genera__FS51__34278B18]  DEFAULT ((0)) FOR [FS51]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF__tbl_genera__FS52__351BAF51]  DEFAULT ((0)) FOR [FS52]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF__tbl_genera__FS53__360FD38A]  DEFAULT ((0)) FOR [FS53]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF__tbl_genera__FS54__3703F7C3]  DEFAULT ((0)) FOR [FS54]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF__tbl_genera__FS55__37F81BFC]  DEFAULT ((0)) FOR [FS55]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF__tbl_genera__FS56__38EC4035]  DEFAULT ((0)) FOR [FS56]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF__tbl_genera__FS57__39E0646E]  DEFAULT ((0)) FOR [FS57]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF__tbl_genera__FS58__3AD488A7]  DEFAULT ((0)) FOR [FS58]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF__tbl_genera__FS59__3BC8ACE0]  DEFAULT ((0)) FOR [FS59]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF__tbl_genera__FS60__3CBCD119]  DEFAULT ((0)) FOR [FS60]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF__tbl_genera__FS61__3DB0F552]  DEFAULT ((0)) FOR [FS61]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF__tbl_genera__FS62__3EA5198B]  DEFAULT ((0)) FOR [FS62]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF__tbl_genera__FS63__3F993DC4]  DEFAULT ((0)) FOR [FS63]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF__tbl_genera__FS64__408D61FD]  DEFAULT ((0)) FOR [FS64]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF__tbl_genera__FS65__41818636]  DEFAULT ((0)) FOR [FS65]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF__tbl_genera__FS66__4275AA6F]  DEFAULT ((0)) FOR [FS66]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF__tbl_genera__FS67__4369CEA8]  DEFAULT ((0)) FOR [FS67]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF__tbl_genera__FS68__445DF2E1]  DEFAULT ((0)) FOR [FS68]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF__tbl_genera__FS69__4552171A]  DEFAULT ((0)) FOR [FS69]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF__tbl_genera__FS70__46463B53]  DEFAULT ((0)) FOR [FS70]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF__tbl_genera__FS71__473A5F8C]  DEFAULT ((0)) FOR [FS71]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF__tbl_genera__FS72__482E83C5]  DEFAULT ((0)) FOR [FS72]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF__tbl_genera__FS73__4922A7FE]  DEFAULT ((0)) FOR [FS73]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF__tbl_genera__FS74__4A16CC37]  DEFAULT ((0)) FOR [FS74]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF__tbl_genera__FS75__4B0AF070]  DEFAULT ((0)) FOR [FS75]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF__tbl_genera__FS76__4BFF14A9]  DEFAULT ((0)) FOR [FS76]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF__tbl_genera__FS77__4CF338E2]  DEFAULT ((0)) FOR [FS77]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF__tbl_genera__FS78__4DE75D1B]  DEFAULT ((0)) FOR [FS78]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF__tbl_genera__FS79__4EDB8154]  DEFAULT ((0)) FOR [FS79]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF__tbl_genera__FS80__4FCFA58D]  DEFAULT ((0)) FOR [FS80]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF__tbl_genera__FS81__50C3C9C6]  DEFAULT ((0)) FOR [FS81]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF__tbl_genera__FS82__51B7EDFF]  DEFAULT ((0)) FOR [FS82]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF__tbl_genera__FS83__52AC1238]  DEFAULT ((0)) FOR [FS83]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF__tbl_genera__FS84__53A03671]  DEFAULT ((0)) FOR [FS84]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF__tbl_genera__FS85__54945AAA]  DEFAULT ((0)) FOR [FS85]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF__tbl_genera__FS86__55887EE3]  DEFAULT ((0)) FOR [FS86]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF__tbl_genera__FS87__567CA31C]  DEFAULT ((0)) FOR [FS87]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF__tbl_genera__FS88__5770C755]  DEFAULT ((0)) FOR [FS88]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF__tbl_genera__FS89__5864EB8E]  DEFAULT ((0)) FOR [FS89]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF__tbl_genera__FS90__59590FC7]  DEFAULT ((0)) FOR [FS90]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF__tbl_genera__FS91__5A4D3400]  DEFAULT ((0)) FOR [FS91]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF__tbl_genera__FS92__5B415839]  DEFAULT ((0)) FOR [FS92]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF__tbl_genera__FS93__5C357C72]  DEFAULT ((0)) FOR [FS93]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF__tbl_genera__FS94__5D29A0AB]  DEFAULT ((0)) FOR [FS94]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF__tbl_genera__FS95__5E1DC4E4]  DEFAULT ((0)) FOR [FS95]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF__tbl_genera__FS96__5F11E91D]  DEFAULT ((0)) FOR [FS96]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF__tbl_genera__FS97__60060D56]  DEFAULT ((0)) FOR [FS97]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF__tbl_genera__FS98__60FA318F]  DEFAULT ((0)) FOR [FS98]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF__tbl_genera__FS99__61EE55C8]  DEFAULT ((0)) FOR [FS99]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF__tbl_gener__MaxLe__75D7B4E9]  DEFAULT ((0)) FOR [MaxLevel]
GO
ALTER TABLE [dbo].[tbl_general] ADD  CONSTRAINT [DF_tbl_general_GuildStatus]  DEFAULT ((0)) FOR [GuildStatus]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_DCK]  DEFAULT ((0)) FOR [DCK]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_StarterBoxMax]  DEFAULT ((0)) FOR [StarterBoxMax]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_BoxItemK_1]  DEFAULT (0xFFFFFFFF) FOR [BoxItemK_1]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_BoxItemMax_1]  DEFAULT ((0)) FOR [BoxItemMax_1]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_LimItemNum_1]  DEFAULT ((0)) FOR [LimItemNum_1]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_BoxItemK_2]  DEFAULT (0xFFFFFFFF) FOR [BoxItemK_2]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_BoxItemMax_2]  DEFAULT ((0)) FOR [BoxItemMax_2]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_LimItemNum_2]  DEFAULT ((0)) FOR [LimItemNum_2]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_K1_0]  DEFAULT (0xFFFFFFFF) FOR [K1_0]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_N1_0]  DEFAULT ((0)) FOR [N1_0]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_K1_1]  DEFAULT (0xFFFFFFFF) FOR [K1_1]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_N1_1]  DEFAULT ((0)) FOR [N1_1]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_K1_2]  DEFAULT (0xFFFFFFFF) FOR [K1_2]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_N1_2]  DEFAULT ((0)) FOR [N1_2]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_K1_3]  DEFAULT (0xFFFFFFFF) FOR [K1_3]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_N1_3]  DEFAULT ((0)) FOR [N1_3]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_K1_4]  DEFAULT (0xFFFFFFFF) FOR [K1_4]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_N1_4]  DEFAULT ((0)) FOR [N1_4]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_K1_5]  DEFAULT (0xFFFFFFFF) FOR [K1_5]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_N1_5]  DEFAULT ((0)) FOR [N1_5]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_K1_6]  DEFAULT (0xFFFFFFFF) FOR [K1_6]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_N1_6]  DEFAULT ((0)) FOR [N1_6]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_K1_7]  DEFAULT (0xFFFFFFFF) FOR [K1_7]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_N1_7]  DEFAULT ((0)) FOR [N1_7]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_K1_8]  DEFAULT (0xFFFFFFFF) FOR [K1_8]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_N1_8]  DEFAULT ((0)) FOR [N1_8]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_K1_9]  DEFAULT (0xFFFFFFFF) FOR [K1_9]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_N1_9]  DEFAULT ((0)) FOR [N1_9]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_K1_10]  DEFAULT (0xFFFFFFFF) FOR [K1_10]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_N1_10]  DEFAULT ((0)) FOR [N1_10]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_K1_11]  DEFAULT (0xFFFFFFFF) FOR [K1_11]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_N1_11]  DEFAULT ((0)) FOR [N1_11]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_K1_12]  DEFAULT (0xFFFFFFFF) FOR [K1_12]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_N1_12]  DEFAULT ((0)) FOR [N1_12]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_K1_13]  DEFAULT (0xFFFFFFFF) FOR [K1_13]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_N1_13]  DEFAULT ((0)) FOR [N1_13]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_K1_14]  DEFAULT (0xFFFFFFFF) FOR [K1_14]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_N1_14]  DEFAULT ((0)) FOR [N1_14]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_K1_15]  DEFAULT (0xFFFFFFFF) FOR [K1_15]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_N1_15]  DEFAULT ((0)) FOR [N1_15]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_K1_16]  DEFAULT (0xFFFFFFFF) FOR [K1_16]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_N1_16]  DEFAULT ((0)) FOR [N1_16]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_K1_17]  DEFAULT (0xFFFFFFFF) FOR [K1_17]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_N1_17]  DEFAULT ((0)) FOR [N1_17]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_K1_18]  DEFAULT (0xFFFFFFFF) FOR [K1_18]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_N1_18]  DEFAULT ((0)) FOR [N1_18]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_K1_19]  DEFAULT (0xFFFFFFFF) FOR [K1_19]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_N1_19]  DEFAULT ((0)) FOR [N1_19]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_K1_20]  DEFAULT (0xFFFFFFFF) FOR [K1_20]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_N1_20]  DEFAULT ((0)) FOR [N1_20]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_K1_21]  DEFAULT (0xFFFFFFFF) FOR [K1_21]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_N1_21]  DEFAULT ((0)) FOR [N1_21]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_K1_22]  DEFAULT (0xFFFFFFFF) FOR [K1_22]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_N1_22]  DEFAULT ((0)) FOR [N1_22]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_K1_23]  DEFAULT (0xFFFFFFFF) FOR [K1_23]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_N1_23]  DEFAULT ((0)) FOR [N1_23]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_K1_24]  DEFAULT (0xFFFFFFFF) FOR [K1_24]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_N1_24]  DEFAULT ((0)) FOR [N1_24]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_K1_25]  DEFAULT (0xFFFFFFFF) FOR [K1_25]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_N1_25]  DEFAULT ((0)) FOR [N1_25]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_K1_26]  DEFAULT (0xFFFFFFFF) FOR [K1_26]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_N1_26]  DEFAULT ((0)) FOR [N1_26]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_K1_27]  DEFAULT (0xFFFFFFFF) FOR [K1_27]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_N1_27]  DEFAULT ((0)) FOR [N1_27]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_K1_28]  DEFAULT (0xFFFFFFFF) FOR [K1_28]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_N1_28]  DEFAULT ((0)) FOR [N1_28]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_K1_29]  DEFAULT (0xFFFFFFFF) FOR [K1_29]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_N1_29]  DEFAULT ((0)) FOR [N1_29]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_K1_30]  DEFAULT (0xFFFFFFFF) FOR [K1_30]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_N1_30]  DEFAULT ((0)) FOR [N1_30]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_K1_31]  DEFAULT (0xFFFFFFFF) FOR [K1_31]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_N1_31]  DEFAULT ((0)) FOR [N1_31]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_K1_32]  DEFAULT (0xFFFFFFFF) FOR [K1_32]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_N1_32]  DEFAULT ((0)) FOR [N1_32]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_K1_33]  DEFAULT (0xFFFFFFFF) FOR [K1_33]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_N1_33]  DEFAULT ((0)) FOR [N1_33]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_K1_34]  DEFAULT (0xFFFFFFFF) FOR [K1_34]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_N1_34]  DEFAULT ((0)) FOR [N1_34]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_K1_35]  DEFAULT (0xFFFFFFFF) FOR [K1_35]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_N1_35]  DEFAULT ((0)) FOR [N1_35]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_K1_36]  DEFAULT (0xFFFFFFFF) FOR [K1_36]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_N1_36]  DEFAULT ((0)) FOR [N1_36]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_K1_37]  DEFAULT (0xFFFFFFFF) FOR [K1_37]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_N1_37]  DEFAULT ((0)) FOR [N1_37]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_K1_38]  DEFAULT (0xFFFFFFFF) FOR [K1_38]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_N1_38]  DEFAULT ((0)) FOR [N1_38]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_K1_39]  DEFAULT (0xFFFFFFFF) FOR [K1_39]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_N1_39]  DEFAULT ((0)) FOR [N1_39]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_K1_40]  DEFAULT (0xFFFFFFFF) FOR [K1_40]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_N1_40]  DEFAULT ((0)) FOR [N1_40]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_K1_41]  DEFAULT (0xFFFFFFFF) FOR [K1_41]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_N1_41]  DEFAULT ((0)) FOR [N1_41]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_K1_42]  DEFAULT (0xFFFFFFFF) FOR [K1_42]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_N1_42]  DEFAULT ((0)) FOR [N1_42]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_K1_43]  DEFAULT (0xFFFFFFFF) FOR [K1_43]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_N1_43]  DEFAULT ((0)) FOR [N1_43]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_K1_44]  DEFAULT (0xFFFFFFFF) FOR [K1_44]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_N1_44]  DEFAULT ((0)) FOR [N1_44]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_K1_45]  DEFAULT (0xFFFFFFFF) FOR [K1_45]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_N1_45]  DEFAULT ((0)) FOR [N1_45]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_K1_46]  DEFAULT (0xFFFFFFFF) FOR [K1_46]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_N1_46]  DEFAULT ((0)) FOR [N1_46]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_K1_47]  DEFAULT (0xFFFFFFFF) FOR [K1_47]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_N1_47]  DEFAULT ((0)) FOR [N1_47]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_K1_48]  DEFAULT (0xFFFFFFFF) FOR [K1_48]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_N1_48]  DEFAULT ((0)) FOR [N1_48]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_K1_49]  DEFAULT (0xFFFFFFFF) FOR [K1_49]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_N1_49]  DEFAULT ((0)) FOR [N1_49]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_K1_50]  DEFAULT (0xFFFFFFFF) FOR [K1_50]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_N1_50]  DEFAULT ((0)) FOR [N1_50]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_K1_51]  DEFAULT (0xFFFFFFFF) FOR [K1_51]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_N1_51]  DEFAULT ((0)) FOR [N1_51]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_K1_52]  DEFAULT (0xFFFFFFFF) FOR [K1_52]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_N1_52]  DEFAULT ((0)) FOR [N1_52]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_K1_53]  DEFAULT (0xFFFFFFFF) FOR [K1_53]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_N1_53]  DEFAULT ((0)) FOR [N1_53]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_K1_54]  DEFAULT (0xFFFFFFFF) FOR [K1_54]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_N1_54]  DEFAULT ((0)) FOR [N1_54]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_K1_55]  DEFAULT (0xFFFFFFFF) FOR [K1_55]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_N1_55]  DEFAULT ((0)) FOR [N1_55]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_K1_56]  DEFAULT (0xFFFFFFFF) FOR [K1_56]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_N1_56]  DEFAULT ((0)) FOR [N1_56]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_K1_57]  DEFAULT (0xFFFFFFFF) FOR [K1_57]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_N1_57]  DEFAULT ((0)) FOR [N1_57]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_K1_58]  DEFAULT (0xFFFFFFFF) FOR [K1_58]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_N1_58]  DEFAULT ((0)) FOR [N1_58]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_K1_59]  DEFAULT (0xFFFFFFFF) FOR [K1_59]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_N1_59]  DEFAULT ((0)) FOR [N1_59]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_K1_60]  DEFAULT (0xFFFFFFFF) FOR [K1_60]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_N1_60]  DEFAULT ((0)) FOR [N1_60]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_K1_61]  DEFAULT (0xFFFFFFFF) FOR [K1_61]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_N1_61]  DEFAULT ((0)) FOR [N1_61]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_K1_62]  DEFAULT (0xFFFFFFFF) FOR [K1_62]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_N1_62]  DEFAULT ((0)) FOR [N1_62]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_K1_63]  DEFAULT (0xFFFFFFFF) FOR [K1_63]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_N1_63]  DEFAULT ((0)) FOR [N1_63]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_K1_64]  DEFAULT (0xFFFFFFFF) FOR [K1_64]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_N1_64]  DEFAULT ((0)) FOR [N1_64]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_K1_65]  DEFAULT (0xFFFFFFFF) FOR [K1_65]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_N1_65]  DEFAULT ((0)) FOR [N1_65]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_K1_66]  DEFAULT (0xFFFFFFFF) FOR [K1_66]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_N1_66]  DEFAULT ((0)) FOR [N1_66]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_K1_67]  DEFAULT (0xFFFFFFFF) FOR [K1_67]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_N1_67]  DEFAULT ((0)) FOR [N1_67]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_K1_68]  DEFAULT (0xFFFFFFFF) FOR [K1_68]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_N1_68]  DEFAULT ((0)) FOR [N1_68]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_K1_69]  DEFAULT (0xFFFFFFFF) FOR [K1_69]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_N1_69]  DEFAULT ((0)) FOR [N1_69]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_K1_70]  DEFAULT (0xFFFFFFFF) FOR [K1_70]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_N1_70]  DEFAULT ((0)) FOR [N1_70]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_K1_71]  DEFAULT (0xFFFFFFFF) FOR [K1_71]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_N1_71]  DEFAULT ((0)) FOR [N1_71]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_K1_72]  DEFAULT (0xFFFFFFFF) FOR [K1_72]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_N1_72]  DEFAULT ((0)) FOR [N1_72]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_K1_73]  DEFAULT (0xFFFFFFFF) FOR [K1_73]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_N1_73]  DEFAULT ((0)) FOR [N1_73]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_K1_74]  DEFAULT (0xFFFFFFFF) FOR [K1_74]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_N1_74]  DEFAULT ((0)) FOR [N1_74]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_K1_75]  DEFAULT (0xFFFFFFFF) FOR [K1_75]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_N1_75]  DEFAULT ((0)) FOR [N1_75]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_K1_76]  DEFAULT (0xFFFFFFFF) FOR [K1_76]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_N1_76]  DEFAULT ((0)) FOR [N1_76]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_K1_77]  DEFAULT (0xFFFFFFFF) FOR [K1_77]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_N1_77]  DEFAULT ((0)) FOR [N1_77]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_K1_78]  DEFAULT (0xFFFFFFFF) FOR [K1_78]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_N1_78]  DEFAULT ((0)) FOR [N1_78]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_K1_79]  DEFAULT (0xFFFFFFFF) FOR [K1_79]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_N1_79]  DEFAULT ((0)) FOR [N1_79]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_K1_80]  DEFAULT (0xFFFFFFFF) FOR [K1_80]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_N1_80]  DEFAULT ((0)) FOR [N1_80]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_K1_81]  DEFAULT (0xFFFFFFFF) FOR [K1_81]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_N1_81]  DEFAULT ((0)) FOR [N1_81]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_K1_82]  DEFAULT (0xFFFFFFFF) FOR [K1_82]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_N1_82]  DEFAULT ((0)) FOR [N1_82]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_K1_83]  DEFAULT (0xFFFFFFFF) FOR [K1_83]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_N1_83]  DEFAULT ((0)) FOR [N1_83]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_K1_84]  DEFAULT (0xFFFFFFFF) FOR [K1_84]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_N1_84]  DEFAULT ((0)) FOR [N1_84]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_K1_85]  DEFAULT (0xFFFFFFFF) FOR [K1_85]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_N1_85]  DEFAULT ((0)) FOR [N1_85]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_K1_86]  DEFAULT (0xFFFFFFFF) FOR [K1_86]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_N1_86]  DEFAULT ((0)) FOR [N1_86]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_K1_87]  DEFAULT (0xFFFFFFFF) FOR [K1_87]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_N1_87]  DEFAULT ((0)) FOR [N1_87]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_K1_88]  DEFAULT (0xFFFFFFFF) FOR [K1_88]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_N1_88]  DEFAULT ((0)) FOR [N1_88]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_K1_89]  DEFAULT (0xFFFFFFFF) FOR [K1_89]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_N1_89]  DEFAULT ((0)) FOR [N1_89]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_K1_90]  DEFAULT (0xFFFFFFFF) FOR [K1_90]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_N1_90]  DEFAULT ((0)) FOR [N1_90]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_K1_91]  DEFAULT (0xFFFFFFFF) FOR [K1_91]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_N1_91]  DEFAULT ((0)) FOR [N1_91]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_K1_92]  DEFAULT (0xFFFFFFFF) FOR [K1_92]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_N1_92]  DEFAULT ((0)) FOR [N1_92]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_K1_93]  DEFAULT (0xFFFFFFFF) FOR [K1_93]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_N1_93]  DEFAULT ((0)) FOR [N1_93]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_K1_94]  DEFAULT (0xFFFFFFFF) FOR [K1_94]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_N1_94]  DEFAULT ((0)) FOR [N1_94]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_K1_95]  DEFAULT (0xFFFFFFFF) FOR [K1_95]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_N1_95]  DEFAULT ((0)) FOR [N1_95]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_K1_96]  DEFAULT (0xFFFFFFFF) FOR [K1_96]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_N1_96]  DEFAULT ((0)) FOR [N1_96]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_K1_97]  DEFAULT (0xFFFFFFFF) FOR [K1_97]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_N1_97]  DEFAULT ((0)) FOR [N1_97]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_K1_98]  DEFAULT (0xFFFFFFFF) FOR [K1_98]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_N1_98]  DEFAULT ((0)) FOR [N1_98]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_K1_99]  DEFAULT (0xFFFFFFFF) FOR [K1_99]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_N1_99]  DEFAULT ((0)) FOR [N1_99]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_K2_0]  DEFAULT (0xFFFFFFFF) FOR [K2_0]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_N2_0]  DEFAULT ((0)) FOR [N2_0]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_K2_1]  DEFAULT (0xFFFFFFFF) FOR [K2_1]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_N2_1]  DEFAULT ((0)) FOR [N2_1]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_K2_2]  DEFAULT (0xFFFFFFFF) FOR [K2_2]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_N2_2]  DEFAULT ((0)) FOR [N2_2]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_K2_3]  DEFAULT (0xFFFFFFFF) FOR [K2_3]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_N2_3]  DEFAULT ((0)) FOR [N2_3]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_K2_4]  DEFAULT (0xFFFFFFFF) FOR [K2_4]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_N2_4]  DEFAULT ((0)) FOR [N2_4]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_K2_5]  DEFAULT (0xFFFFFFFF) FOR [K2_5]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_N2_5]  DEFAULT ((0)) FOR [N2_5]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_K2_6]  DEFAULT (0xFFFFFFFF) FOR [K2_6]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_N2_6]  DEFAULT ((0)) FOR [N2_6]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_K2_7]  DEFAULT (0xFFFFFFFF) FOR [K2_7]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_N2_7]  DEFAULT ((0)) FOR [N2_7]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_K2_8]  DEFAULT (0xFFFFFFFF) FOR [K2_8]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_N2_8]  DEFAULT ((0)) FOR [N2_8]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_K2_9]  DEFAULT (0xFFFFFFFF) FOR [K2_9]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_N2_9]  DEFAULT ((0)) FOR [N2_9]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_K2_10]  DEFAULT (0xFFFFFFFF) FOR [K2_10]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_N2_10]  DEFAULT ((0)) FOR [N2_10]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_K2_11]  DEFAULT (0xFFFFFFFF) FOR [K2_11]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_N2_11]  DEFAULT ((0)) FOR [N2_11]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_K2_12]  DEFAULT (0xFFFFFFFF) FOR [K2_12]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_N2_12]  DEFAULT ((0)) FOR [N2_12]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_K2_13]  DEFAULT (0xFFFFFFFF) FOR [K2_13]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_N2_13]  DEFAULT ((0)) FOR [N2_13]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_K2_14]  DEFAULT (0xFFFFFFFF) FOR [K2_14]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_N2_14]  DEFAULT ((0)) FOR [N2_14]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_K2_15]  DEFAULT (0xFFFFFFFF) FOR [K2_15]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_N2_15]  DEFAULT ((0)) FOR [N2_15]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_K2_16]  DEFAULT (0xFFFFFFFF) FOR [K2_16]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_N2_16]  DEFAULT ((0)) FOR [N2_16]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_K2_17]  DEFAULT (0xFFFFFFFF) FOR [K2_17]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_N2_17]  DEFAULT ((0)) FOR [N2_17]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_K2_18]  DEFAULT (0xFFFFFFFF) FOR [K2_18]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_N2_18]  DEFAULT ((0)) FOR [N2_18]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_K2_19]  DEFAULT (0xFFFFFFFF) FOR [K2_19]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_N2_19]  DEFAULT ((0)) FOR [N2_19]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_K2_20]  DEFAULT (0xFFFFFFFF) FOR [K2_20]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_N2_20]  DEFAULT ((0)) FOR [N2_20]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_K2_21]  DEFAULT (0xFFFFFFFF) FOR [K2_21]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_N2_21]  DEFAULT ((0)) FOR [N2_21]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_K2_22]  DEFAULT (0xFFFFFFFF) FOR [K2_22]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_N2_22]  DEFAULT ((0)) FOR [N2_22]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_K2_23]  DEFAULT (0xFFFFFFFF) FOR [K2_23]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_N2_23]  DEFAULT ((0)) FOR [N2_23]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_K2_24]  DEFAULT (0xFFFFFFFF) FOR [K2_24]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_N2_24]  DEFAULT ((0)) FOR [N2_24]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_K2_25]  DEFAULT (0xFFFFFFFF) FOR [K2_25]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_N2_25]  DEFAULT ((0)) FOR [N2_25]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_K2_26]  DEFAULT (0xFFFFFFFF) FOR [K2_26]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_N2_26]  DEFAULT ((0)) FOR [N2_26]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_K2_27]  DEFAULT (0xFFFFFFFF) FOR [K2_27]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_N2_27]  DEFAULT ((0)) FOR [N2_27]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_K2_28]  DEFAULT (0xFFFFFFFF) FOR [K2_28]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_N2_28]  DEFAULT ((0)) FOR [N2_28]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_K2_29]  DEFAULT (0xFFFFFFFF) FOR [K2_29]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_N2_29]  DEFAULT ((0)) FOR [N2_29]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_K2_30]  DEFAULT (0xFFFFFFFF) FOR [K2_30]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_N2_30]  DEFAULT ((0)) FOR [N2_30]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_K2_31]  DEFAULT (0xFFFFFFFF) FOR [K2_31]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_N2_31]  DEFAULT ((0)) FOR [N2_31]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_K2_32]  DEFAULT (0xFFFFFFFF) FOR [K2_32]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_N2_32]  DEFAULT ((0)) FOR [N2_32]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_K2_33]  DEFAULT (0xFFFFFFFF) FOR [K2_33]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_N2_33]  DEFAULT ((0)) FOR [N2_33]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_K2_34]  DEFAULT (0xFFFFFFFF) FOR [K2_34]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_N2_34]  DEFAULT ((0)) FOR [N2_34]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_K2_35]  DEFAULT (0xFFFFFFFF) FOR [K2_35]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_N2_35]  DEFAULT ((0)) FOR [N2_35]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_K2_36]  DEFAULT (0xFFFFFFFF) FOR [K2_36]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_N2_36]  DEFAULT ((0)) FOR [N2_36]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_K2_37]  DEFAULT (0xFFFFFFFF) FOR [K2_37]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_N2_37]  DEFAULT ((0)) FOR [N2_37]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_K2_38]  DEFAULT (0xFFFFFFFF) FOR [K2_38]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_N2_38]  DEFAULT ((0)) FOR [N2_38]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_K2_39]  DEFAULT (0xFFFFFFFF) FOR [K2_39]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_N2_39]  DEFAULT ((0)) FOR [N2_39]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_K2_40]  DEFAULT (0xFFFFFFFF) FOR [K2_40]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_N2_40]  DEFAULT ((0)) FOR [N2_40]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_K2_41]  DEFAULT (0xFFFFFFFF) FOR [K2_41]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_N2_41]  DEFAULT ((0)) FOR [N2_41]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_K2_42]  DEFAULT (0xFFFFFFFF) FOR [K2_42]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_N2_42]  DEFAULT ((0)) FOR [N2_42]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_K2_43]  DEFAULT (0xFFFFFFFF) FOR [K2_43]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_N2_43]  DEFAULT ((0)) FOR [N2_43]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_K2_44]  DEFAULT (0xFFFFFFFF) FOR [K2_44]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_N2_44]  DEFAULT ((0)) FOR [N2_44]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_K2_45]  DEFAULT (0xFFFFFFFF) FOR [K2_45]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_N2_45]  DEFAULT ((0)) FOR [N2_45]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_K2_46]  DEFAULT (0xFFFFFFFF) FOR [K2_46]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_N2_46]  DEFAULT ((0)) FOR [N2_46]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_K2_47]  DEFAULT (0xFFFFFFFF) FOR [K2_47]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_N2_47]  DEFAULT ((0)) FOR [N2_47]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_K2_48]  DEFAULT (0xFFFFFFFF) FOR [K2_48]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_N2_48]  DEFAULT ((0)) FOR [N2_48]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_K2_49]  DEFAULT (0xFFFFFFFF) FOR [K2_49]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_N2_49]  DEFAULT ((0)) FOR [N2_49]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_K2_50]  DEFAULT (0xFFFFFFFF) FOR [K2_50]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_N2_50]  DEFAULT ((0)) FOR [N2_50]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_K2_51]  DEFAULT (0xFFFFFFFF) FOR [K2_51]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_N2_51]  DEFAULT ((0)) FOR [N2_51]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_K2_52]  DEFAULT (0xFFFFFFFF) FOR [K2_52]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_N2_52]  DEFAULT ((0)) FOR [N2_52]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_K2_53]  DEFAULT (0xFFFFFFFF) FOR [K2_53]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_N2_53]  DEFAULT ((0)) FOR [N2_53]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_K2_54]  DEFAULT (0xFFFFFFFF) FOR [K2_54]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_N2_54]  DEFAULT ((0)) FOR [N2_54]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_K2_55]  DEFAULT (0xFFFFFFFF) FOR [K2_55]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_N2_55]  DEFAULT ((0)) FOR [N2_55]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_K2_56]  DEFAULT (0xFFFFFFFF) FOR [K2_56]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_N2_56]  DEFAULT ((0)) FOR [N2_56]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_K2_57]  DEFAULT (0xFFFFFFFF) FOR [K2_57]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_N2_57]  DEFAULT ((0)) FOR [N2_57]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_K2_58]  DEFAULT (0xFFFFFFFF) FOR [K2_58]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_N2_58]  DEFAULT ((0)) FOR [N2_58]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_K2_59]  DEFAULT (0xFFFFFFFF) FOR [K2_59]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_N2_59]  DEFAULT ((0)) FOR [N2_59]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_K2_60]  DEFAULT (0xFFFFFFFF) FOR [K2_60]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_N2_60]  DEFAULT ((0)) FOR [N2_60]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_K2_61]  DEFAULT (0xFFFFFFFF) FOR [K2_61]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_N2_61]  DEFAULT ((0)) FOR [N2_61]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_K2_62]  DEFAULT (0xFFFFFFFF) FOR [K2_62]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_N2_62]  DEFAULT ((0)) FOR [N2_62]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_K2_63]  DEFAULT (0xFFFFFFFF) FOR [K2_63]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_N2_63]  DEFAULT ((0)) FOR [N2_63]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_K2_64]  DEFAULT (0xFFFFFFFF) FOR [K2_64]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_N2_64]  DEFAULT ((0)) FOR [N2_64]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_K2_65]  DEFAULT (0xFFFFFFFF) FOR [K2_65]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_N2_65]  DEFAULT ((0)) FOR [N2_65]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_K2_66]  DEFAULT (0xFFFFFFFF) FOR [K2_66]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_N2_66]  DEFAULT ((0)) FOR [N2_66]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_K2_67]  DEFAULT (0xFFFFFFFF) FOR [K2_67]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_N2_67]  DEFAULT ((0)) FOR [N2_67]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_K2_68]  DEFAULT (0xFFFFFFFF) FOR [K2_68]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_N2_68]  DEFAULT ((0)) FOR [N2_68]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_K2_69]  DEFAULT (0xFFFFFFFF) FOR [K2_69]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_N2_69]  DEFAULT ((0)) FOR [N2_69]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_K2_70]  DEFAULT (0xFFFFFFFF) FOR [K2_70]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_N2_70]  DEFAULT ((0)) FOR [N2_70]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_K2_71]  DEFAULT (0xFFFFFFFF) FOR [K2_71]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_N2_71]  DEFAULT ((0)) FOR [N2_71]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_K2_72]  DEFAULT (0xFFFFFFFF) FOR [K2_72]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_N2_72]  DEFAULT ((0)) FOR [N2_72]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_K2_73]  DEFAULT (0xFFFFFFFF) FOR [K2_73]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_N2_73]  DEFAULT ((0)) FOR [N2_73]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_K2_74]  DEFAULT (0xFFFFFFFF) FOR [K2_74]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_N2_74]  DEFAULT ((0)) FOR [N2_74]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_K2_75]  DEFAULT (0xFFFFFFFF) FOR [K2_75]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_N2_75]  DEFAULT ((0)) FOR [N2_75]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_K2_76]  DEFAULT (0xFFFFFFFF) FOR [K2_76]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_N2_76]  DEFAULT ((0)) FOR [N2_76]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_K2_77]  DEFAULT (0xFFFFFFFF) FOR [K2_77]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_N2_77]  DEFAULT ((0)) FOR [N2_77]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_K2_78]  DEFAULT (0xFFFFFFFF) FOR [K2_78]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_N2_78]  DEFAULT ((0)) FOR [N2_78]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_K2_79]  DEFAULT (0xFFFFFFFF) FOR [K2_79]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_N2_79]  DEFAULT ((0)) FOR [N2_79]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_K2_80]  DEFAULT (0xFFFFFFFF) FOR [K2_80]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_N2_80]  DEFAULT ((0)) FOR [N2_80]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_K2_81]  DEFAULT (0xFFFFFFFF) FOR [K2_81]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_N2_81]  DEFAULT ((0)) FOR [N2_81]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_K2_82]  DEFAULT (0xFFFFFFFF) FOR [K2_82]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_N2_82]  DEFAULT ((0)) FOR [N2_82]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_K2_83]  DEFAULT (0xFFFFFFFF) FOR [K2_83]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_N2_83]  DEFAULT ((0)) FOR [N2_83]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_K2_84]  DEFAULT (0xFFFFFFFF) FOR [K2_84]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_N2_84]  DEFAULT ((0)) FOR [N2_84]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_K2_85]  DEFAULT (0xFFFFFFFF) FOR [K2_85]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_N2_85]  DEFAULT ((0)) FOR [N2_85]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_K2_86]  DEFAULT (0xFFFFFFFF) FOR [K2_86]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_N2_86]  DEFAULT ((0)) FOR [N2_86]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_K2_87]  DEFAULT (0xFFFFFFFF) FOR [K2_87]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_N2_87]  DEFAULT ((0)) FOR [N2_87]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_K2_88]  DEFAULT (0xFFFFFFFF) FOR [K2_88]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_N2_88]  DEFAULT ((0)) FOR [N2_88]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_K2_89]  DEFAULT (0xFFFFFFFF) FOR [K2_89]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_N2_89]  DEFAULT ((0)) FOR [N2_89]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_K2_90]  DEFAULT (0xFFFFFFFF) FOR [K2_90]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_N2_90]  DEFAULT ((0)) FOR [N2_90]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_K2_91]  DEFAULT (0xFFFFFFFF) FOR [K2_91]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_N2_91]  DEFAULT ((0)) FOR [N2_91]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_K2_92]  DEFAULT (0xFFFFFFFF) FOR [K2_92]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_N2_92]  DEFAULT ((0)) FOR [N2_92]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_K2_93]  DEFAULT (0xFFFFFFFF) FOR [K2_93]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_N2_93]  DEFAULT ((0)) FOR [N2_93]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_K2_94]  DEFAULT (0xFFFFFFFF) FOR [K2_94]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_N2_94]  DEFAULT ((0)) FOR [N2_94]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_K2_95]  DEFAULT (0xFFFFFFFF) FOR [K2_95]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_N2_95]  DEFAULT ((0)) FOR [N2_95]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_K2_96]  DEFAULT (0xFFFFFFFF) FOR [K2_96]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_N2_96]  DEFAULT ((0)) FOR [N2_96]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_K2_97]  DEFAULT (0xFFFFFFFF) FOR [K2_97]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_N2_97]  DEFAULT ((0)) FOR [N2_97]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_K2_98]  DEFAULT (0xFFFFFFFF) FOR [K2_98]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_N2_98]  DEFAULT ((0)) FOR [N2_98]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_K2_99]  DEFAULT (0xFFFFFFFF) FOR [K2_99]
GO
ALTER TABLE [dbo].[tbl_GoldBoxItem] ADD  CONSTRAINT [DF_tbl_GoldBoxItem_N2_99]  DEFAULT ((0)) FOR [N2_99]
GO
ALTER TABLE [dbo].[tbl_GreetMsg] ADD  CONSTRAINT [DF__tbl_GreetMsg_Name]  DEFAULT (' ') FOR [Name]
GO
ALTER TABLE [dbo].[tbl_Guild] ADD  CONSTRAINT [DF_tbl_Guild_Profile]  DEFAULT ('??') FOR [Profile]
GO
ALTER TABLE [dbo].[tbl_Guild] ADD  CONSTRAINT [DF_tbl_Guild_createDt]  DEFAULT (getdate()) FOR [CreateDt]
GO
ALTER TABLE [dbo].[tbl_Guild] ADD  CONSTRAINT [DF_tbl_Guild_DCK]  DEFAULT ((1)) FOR [Grade]
GO
ALTER TABLE [dbo].[tbl_Guild] ADD  CONSTRAINT [DF_tbl_Guild_DCK1]  DEFAULT ((0)) FOR [DCK]
GO
ALTER TABLE [dbo].[tbl_Guild] ADD  CONSTRAINT [DF_tbl_Guild_Dalant]  DEFAULT ((0)) FOR [Dalant]
GO
ALTER TABLE [dbo].[tbl_Guild] ADD  CONSTRAINT [DF_tbl_Guild_Gold]  DEFAULT ((0)) FOR [Gold]
GO
ALTER TABLE [dbo].[tbl_Guild] ADD  CONSTRAINT [DF_tbl_Guild_EmblemBack]  DEFAULT (0xFFFFFFFF) FOR [EmblemBack]
GO
ALTER TABLE [dbo].[tbl_Guild] ADD  CONSTRAINT [DF_tbl_Guild_EmblemMark]  DEFAULT (0xFFFFFFFF) FOR [EmblemMark]
GO
ALTER TABLE [dbo].[tbl_Guild] ADD  CONSTRAINT [DF_tbl_guild_race]  DEFAULT ((255)) FOR [Race]
GO
ALTER TABLE [dbo].[tbl_Guild] ADD  CONSTRAINT [DF_tbl_Guild_MemberCount]  DEFAULT ((8)) FOR [MemberCount]
GO
ALTER TABLE [dbo].[tbl_Guild] ADD  CONSTRAINT [DF__tbl_Guild__Maste__2F25DA6B]  DEFAULT (0xFFFFFFFF) FOR [MasterSerial]
GO
ALTER TABLE [dbo].[tbl_Guild] ADD  CONSTRAINT [DF__tbl_Guild__Maste__3019FEA4]  DEFAULT (0xFF) FOR [MasterBeforeGrade]
GO
ALTER TABLE [dbo].[tbl_Guild] ADD  CONSTRAINT [DF_tbl_Guild_GMsg]  DEFAULT ('') FOR [GMsg]
GO
ALTER TABLE [dbo].[tbl_Guild] ADD  CONSTRAINT [DF_deleteid]  DEFAULT ('*') FOR [deleteid]
GO
ALTER TABLE [dbo].[tbl_GuildBattleRank] ADD  CONSTRAINT [DF_tbl_GuildRank_dck]  DEFAULT ((0)) FOR [dck]
GO
ALTER TABLE [dbo].[tbl_GuildBattleRank] ADD  CONSTRAINT [DF_tbl_GuildRank_win]  DEFAULT ((0)) FOR [win]
GO
ALTER TABLE [dbo].[tbl_GuildBattleRank] ADD  CONSTRAINT [DF_tbl_GuildRank_lose]  DEFAULT ((0)) FOR [lose]
GO
ALTER TABLE [dbo].[tbl_GuildBattleRank] ADD  CONSTRAINT [DF_tbl_GuildRank_draw]  DEFAULT ((0)) FOR [draw]
GO
ALTER TABLE [dbo].[tbl_GuildBattleRank] ADD  CONSTRAINT [DF_tbl_GuildRank_score]  DEFAULT ((0)) FOR [score]
GO
ALTER TABLE [dbo].[tbl_GuildBattleRank] ADD  CONSTRAINT [DF_tbl_GuildRank_totwin]  DEFAULT ((0)) FOR [totwin]
GO
ALTER TABLE [dbo].[tbl_GuildBattleRank] ADD  CONSTRAINT [DF_tbl_GuildRank_totloset]  DEFAULT ((0)) FOR [totlose]
GO
ALTER TABLE [dbo].[tbl_GuildBattleRank] ADD  CONSTRAINT [DF_tbl_GuildRank_totdraw]  DEFAULT ((0)) FOR [totdraw]
GO
ALTER TABLE [dbo].[tbl_GuildBattleRank] ADD  CONSTRAINT [DF_tbl_GuildRank_totcnt]  DEFAULT ((0)) FOR [totcnt]
GO
ALTER TABLE [dbo].[tbl_guildbattleresullog] ADD  CONSTRAINT [DF_tbl_guildbattleresullog_redweeklywin]  DEFAULT ((0)) FOR [redweeklywin]
GO
ALTER TABLE [dbo].[tbl_guildbattleresullog] ADD  CONSTRAINT [DF_tbl_guildbattleresullog_redweeklylose]  DEFAULT ((0)) FOR [redweeklylose]
GO
ALTER TABLE [dbo].[tbl_guildbattleresullog] ADD  CONSTRAINT [DF_tbl_guildbattleresullog_redweeklydraw]  DEFAULT ((0)) FOR [redweeklydraw]
GO
ALTER TABLE [dbo].[tbl_guildbattleresullog] ADD  CONSTRAINT [DF_tbl_guildbattleresullog_redweeklywinscore]  DEFAULT ((0)) FOR [redweeklywinscore]
GO
ALTER TABLE [dbo].[tbl_guildbattleresullog] ADD  CONSTRAINT [DF_tbl_guildbattleresullog_redtotwin]  DEFAULT ((0)) FOR [redtotwin]
GO
ALTER TABLE [dbo].[tbl_guildbattleresullog] ADD  CONSTRAINT [DF_tbl_guildbattleresullog_redtotlose]  DEFAULT ((0)) FOR [redtotlose]
GO
ALTER TABLE [dbo].[tbl_guildbattleresullog] ADD  CONSTRAINT [DF_tbl_guildbattleresullog_redtotdraw]  DEFAULT ((0)) FOR [redtotdraw]
GO
ALTER TABLE [dbo].[tbl_guildbattleresullog] ADD  CONSTRAINT [DF_tbl_guildbattleresullog_redtotbattlecnt]  DEFAULT ((0)) FOR [redtotbattlecnt]
GO
ALTER TABLE [dbo].[tbl_guildbattleresullog] ADD  CONSTRAINT [DF_tbl_guildbattleresullog_blueweeklywin]  DEFAULT ((0)) FOR [blueweeklywin]
GO
ALTER TABLE [dbo].[tbl_guildbattleresullog] ADD  CONSTRAINT [DF_tbl_guildbattleresullog_blueweeklylose]  DEFAULT ((0)) FOR [blueweeklylose]
GO
ALTER TABLE [dbo].[tbl_guildbattleresullog] ADD  CONSTRAINT [DF_tbl_guildbattleresullog_blueweeklydraw]  DEFAULT ((0)) FOR [blueweeklydraw]
GO
ALTER TABLE [dbo].[tbl_guildbattleresullog] ADD  CONSTRAINT [DF_tbl_guildbattleresullog_blueweeklywinscore]  DEFAULT ((0)) FOR [blueweeklywinscore]
GO
ALTER TABLE [dbo].[tbl_guildbattleresullog] ADD  CONSTRAINT [DF_tbl_guildbattleresullog_bluetotwin]  DEFAULT ((0)) FOR [bluetotwin]
GO
ALTER TABLE [dbo].[tbl_guildbattleresullog] ADD  CONSTRAINT [DF_tbl_guildbattleresullog_bluetotlose]  DEFAULT ((0)) FOR [bluetotlose]
GO
ALTER TABLE [dbo].[tbl_guildbattleresullog] ADD  CONSTRAINT [DF_tbl_guildbattleresullog_bluetotdraw]  DEFAULT ((0)) FOR [bluetotdraw]
GO
ALTER TABLE [dbo].[tbl_guildbattleresullog] ADD  CONSTRAINT [DF_tbl_guildbattleresullog_bluetotbattlecnt]  DEFAULT ((0)) FOR [bluetotbattlecnt]
GO
ALTER TABLE [dbo].[tbl_guildbattleresullog] ADD  CONSTRAINT [DF_tbl_guildbattleresullog_redguildbattlepvppoint]  DEFAULT ((0)) FOR [redguildbattlepvppoint]
GO
ALTER TABLE [dbo].[tbl_guildbattleresullog] ADD  CONSTRAINT [DF_tbl_guildbattleresullog_blueguildbattlepvppoint]  DEFAULT ((0)) FOR [blueguildbattlepvppoint]
GO
ALTER TABLE [dbo].[tbl_GuildRoom] ADD  CONSTRAINT [DF_tbl_GuildRoom_Dck]  DEFAULT ((0)) FOR [Dck]
GO
ALTER TABLE [dbo].[tbl_GuildRoom] ADD  CONSTRAINT [DF_tbl_GuildRoom_GuildSerial]  DEFAULT ((0)) FOR [GuildSerial]
GO
ALTER TABLE [dbo].[tbl_GuildRoom] ADD  CONSTRAINT [DF_tbl_GuildRoom_RoomType]  DEFAULT ((0)) FOR [RoomType]
GO
ALTER TABLE [dbo].[tbl_GuildRoom] ADD  CONSTRAINT [DF_tbl_GuildRoom_RoomRace]  DEFAULT ((0)) FOR [RoomRace]
GO
ALTER TABLE [dbo].[tbl_GuildRoom] ADD  CONSTRAINT [DF_tbl_GuildRoom_RoomRentDate]  DEFAULT ('GetDate()') FOR [RoomRentDate]
GO
ALTER TABLE [dbo].[tbl_GuildRoom] ADD  CONSTRAINT [DF_tbl_GuildRoom_LogDate]  DEFAULT ('GetDate()') FOR [LogDate]
GO
ALTER TABLE [dbo].[tbl_history_electraceboss_0] ADD  CONSTRAINT [DF_tbl_history_electraceboss_0_WorldName]  DEFAULT ('*') FOR [WorldName]
GO
ALTER TABLE [dbo].[tbl_history_electraceboss_0] ADD  CONSTRAINT [DF_tbl_history_electraceboss_0_tmCreate]  DEFAULT (getdate()) FOR [tmCreate]
GO
ALTER TABLE [dbo].[tbl_history_electraceboss_0] ADD  CONSTRAINT [DF_tbl_history_electraceboss_0_bOnElect]  DEFAULT ((1)) FOR [bOnElect]
GO
ALTER TABLE [dbo].[tbl_history_electraceboss_0] ADD  CONSTRAINT [DF_tbl_history_electraceboss_0_bOnCandidate]  DEFAULT ((0)) FOR [bOnCandidate]
GO
ALTER TABLE [dbo].[tbl_history_electraceboss_0] ADD  CONSTRAINT [DF_tbl_history_electraceboss_0_bOnVote]  DEFAULT ((0)) FOR [bOnVote]
GO
ALTER TABLE [dbo].[tbl_history_electraceboss_0] ADD  CONSTRAINT [DF_tbl_history_electraceboss_0_tmOnElect]  DEFAULT (getdate()) FOR [tmOnElect]
GO
ALTER TABLE [dbo].[tbl_history_electraceboss_0] ADD  CONSTRAINT [DF_tbl_history_electraceboss_0_tmOnCandiate]  DEFAULT (getdate()) FOR [tmOnCandidate]
GO
ALTER TABLE [dbo].[tbl_history_electraceboss_0] ADD  CONSTRAINT [DF_tbl_history_electraceboss_0_tmOnVote]  DEFAULT (getdate()) FOR [tmOnVote]
GO
ALTER TABLE [dbo].[tbl_history_electraceboss_0] ADD  CONSTRAINT [DF_tbl_history_electraceboss_0_TotalVoteCnt]  DEFAULT ((0)) FOR [TotalVoteCnt]
GO
ALTER TABLE [dbo].[tbl_history_electraceboss_0] ADD  CONSTRAINT [DF_tbl_history_electraceboss_0_tmLastUpdate]  DEFAULT (getdate()) FOR [tmLastUpdate]
GO
ALTER TABLE [dbo].[tbl_history_electraceboss_1] ADD  CONSTRAINT [DF_tbl_history_electraceboss_1_WorldName]  DEFAULT ('*') FOR [WorldName]
GO
ALTER TABLE [dbo].[tbl_history_electraceboss_1] ADD  CONSTRAINT [DF_tbl_history_electraceboss_1_tmCreate]  DEFAULT (getdate()) FOR [tmCreate]
GO
ALTER TABLE [dbo].[tbl_history_electraceboss_1] ADD  CONSTRAINT [DF_tbl_history_electraceboss_1_bOnElect]  DEFAULT ((1)) FOR [bOnElect]
GO
ALTER TABLE [dbo].[tbl_history_electraceboss_1] ADD  CONSTRAINT [DF_tbl_history_electraceboss_1_bOnCandidate]  DEFAULT ((0)) FOR [bOnCandidate]
GO
ALTER TABLE [dbo].[tbl_history_electraceboss_1] ADD  CONSTRAINT [DF_tbl_history_electraceboss_1_bOnVote]  DEFAULT ((0)) FOR [bOnVote]
GO
ALTER TABLE [dbo].[tbl_history_electraceboss_1] ADD  CONSTRAINT [DF_tbl_history_electraceboss_1_tmOnElect]  DEFAULT (getdate()) FOR [tmOnElect]
GO
ALTER TABLE [dbo].[tbl_history_electraceboss_1] ADD  CONSTRAINT [DF_tbl_history_electraceboss_1_tmOnCandiate]  DEFAULT (getdate()) FOR [tmOnCandidate]
GO
ALTER TABLE [dbo].[tbl_history_electraceboss_1] ADD  CONSTRAINT [DF_tbl_history_electraceboss_1_tmOnVote]  DEFAULT (getdate()) FOR [tmOnVote]
GO
ALTER TABLE [dbo].[tbl_history_electraceboss_1] ADD  CONSTRAINT [DF_tbl_history_electraceboss_1_TotalVoteCnt]  DEFAULT ((0)) FOR [TotalVoteCnt]
GO
ALTER TABLE [dbo].[tbl_history_electraceboss_1] ADD  CONSTRAINT [DF_tbl_history_electraceboss_1_tmLastUpdate]  DEFAULT (getdate()) FOR [tmLastUpdate]
GO
ALTER TABLE [dbo].[tbl_history_electraceboss_2] ADD  CONSTRAINT [DF_tbl_history_electraceboss_2_WorldName]  DEFAULT ('*') FOR [WorldName]
GO
ALTER TABLE [dbo].[tbl_history_electraceboss_2] ADD  CONSTRAINT [DF_tbl_history_electraceboss_2_tmCreate]  DEFAULT (getdate()) FOR [tmCreate]
GO
ALTER TABLE [dbo].[tbl_history_electraceboss_2] ADD  CONSTRAINT [DF_tbl_history_electraceboss_2_bOnElect]  DEFAULT ((1)) FOR [bOnElect]
GO
ALTER TABLE [dbo].[tbl_history_electraceboss_2] ADD  CONSTRAINT [DF_tbl_history_electraceboss_2_bOnCandidate]  DEFAULT ((0)) FOR [bOnCandidate]
GO
ALTER TABLE [dbo].[tbl_history_electraceboss_2] ADD  CONSTRAINT [DF_tbl_history_electraceboss_2_bOnVote]  DEFAULT ((0)) FOR [bOnVote]
GO
ALTER TABLE [dbo].[tbl_history_electraceboss_2] ADD  CONSTRAINT [DF_tbl_history_electraceboss_2_tmOnElect]  DEFAULT (getdate()) FOR [tmOnElect]
GO
ALTER TABLE [dbo].[tbl_history_electraceboss_2] ADD  CONSTRAINT [DF_tbl_history_electraceboss_2_tmOnCandiate]  DEFAULT (getdate()) FOR [tmOnCandidate]
GO
ALTER TABLE [dbo].[tbl_history_electraceboss_2] ADD  CONSTRAINT [DF_tbl_history_electraceboss_2_tmOnVote]  DEFAULT (getdate()) FOR [tmOnVote]
GO
ALTER TABLE [dbo].[tbl_history_electraceboss_2] ADD  CONSTRAINT [DF_tbl_history_electraceboss_2_TotalVoteCnt]  DEFAULT ((0)) FOR [TotalVoteCnt]
GO
ALTER TABLE [dbo].[tbl_history_electraceboss_2] ADD  CONSTRAINT [DF_tbl_history_electraceboss_2_tmLastUpdate]  DEFAULT (getdate()) FOR [tmLastUpdate]
GO
ALTER TABLE [dbo].[tbl_history_electraceboss_candidacy_0] ADD  CONSTRAINT [DF_tbl_history_electraceboss_candidacy_0_bGiveback]  DEFAULT ((0)) FOR [bGiveback]
GO
ALTER TABLE [dbo].[tbl_history_electraceboss_candidacy_0] ADD  CONSTRAINT [DF_tbl_history_electraceboss_candidacy_0_bPrevBoss]  DEFAULT ((0)) FOR [bPrevBoss]
GO
ALTER TABLE [dbo].[tbl_history_electraceboss_candidacy_0] ADD  CONSTRAINT [DF_tbl_history_electraceboss_candidacy_0_bregSecond]  DEFAULT ((0)) FOR [bregSecond]
GO
ALTER TABLE [dbo].[tbl_history_electraceboss_candidacy_0] ADD  CONSTRAINT [DF_tbl_history_electraceboss_candidacy_0_tmReg_F]  DEFAULT (getdate()) FOR [tmReg_F]
GO
ALTER TABLE [dbo].[tbl_history_electraceboss_candidacy_0] ADD  CONSTRAINT [DF_tbl_history_electraceboss_candidacy_0_tmReg_S]  DEFAULT (getdate()) FOR [tmReg_S]
GO
ALTER TABLE [dbo].[tbl_history_electraceboss_candidacy_0] ADD  CONSTRAINT [DF_tbl_history_electraceboss_candidacy_0_GuildSerial]  DEFAULT ((-1)) FOR [GuildSerial]
GO
ALTER TABLE [dbo].[tbl_history_electraceboss_candidacy_0] ADD  CONSTRAINT [DF_tbl_history_electraceboss_candidacy_0_GuildName]  DEFAULT (N'(*)') FOR [GuildName]
GO
ALTER TABLE [dbo].[tbl_history_electraceboss_candidacy_0] ADD  CONSTRAINT [DF_tbl_history_electraceboss_candidacy_0_Rank]  DEFAULT ((0)) FOR [Rank]
GO
ALTER TABLE [dbo].[tbl_history_electraceboss_candidacy_0] ADD  CONSTRAINT [DF_tbl_history_electraceboss_candidacy_0_Score]  DEFAULT ((0)) FOR [Score]
GO
ALTER TABLE [dbo].[tbl_history_electraceboss_candidacy_0] ADD  CONSTRAINT [DF_tbl_history_electraceboss_candidacy_0_WinCnt]  DEFAULT ((0)) FOR [WinCnt]
GO
ALTER TABLE [dbo].[tbl_history_electraceboss_candidacy_1] ADD  CONSTRAINT [DF_tbl_history_electraceboss_candidacy_1_bGiveback]  DEFAULT ((0)) FOR [bGiveback]
GO
ALTER TABLE [dbo].[tbl_history_electraceboss_candidacy_1] ADD  CONSTRAINT [DF_tbl_history_electraceboss_candidacy_1_bPrevBoss]  DEFAULT ((0)) FOR [bPrevBoss]
GO
ALTER TABLE [dbo].[tbl_history_electraceboss_candidacy_1] ADD  CONSTRAINT [DF_tbl_history_electraceboss_candidacy_1_bregSecond]  DEFAULT ((0)) FOR [bregSecond]
GO
ALTER TABLE [dbo].[tbl_history_electraceboss_candidacy_1] ADD  CONSTRAINT [DF_tbl_history_electraceboss_candidacy_1_tmReg_F]  DEFAULT (getdate()) FOR [tmReg_F]
GO
ALTER TABLE [dbo].[tbl_history_electraceboss_candidacy_1] ADD  CONSTRAINT [DF_tbl_history_electraceboss_candidacy_1_tmReg_S]  DEFAULT (getdate()) FOR [tmReg_S]
GO
ALTER TABLE [dbo].[tbl_history_electraceboss_candidacy_1] ADD  CONSTRAINT [DF_tbl_history_electraceboss_candidacy_1_GuildSerial]  DEFAULT ((-1)) FOR [GuildSerial]
GO
ALTER TABLE [dbo].[tbl_history_electraceboss_candidacy_1] ADD  CONSTRAINT [DF_tbl_history_electraceboss_candidacy_1_GuildName]  DEFAULT (N'(*)') FOR [GuildName]
GO
ALTER TABLE [dbo].[tbl_history_electraceboss_candidacy_1] ADD  CONSTRAINT [DF_tbl_history_electraceboss_candidacy_1_Rank]  DEFAULT ((0)) FOR [Rank]
GO
ALTER TABLE [dbo].[tbl_history_electraceboss_candidacy_1] ADD  CONSTRAINT [DF_tbl_history_electraceboss_candidacy_1_Score]  DEFAULT ((0)) FOR [Score]
GO
ALTER TABLE [dbo].[tbl_history_electraceboss_candidacy_1] ADD  CONSTRAINT [DF_tbl_history_electraceboss_candidacy_1_WinCnt]  DEFAULT ((0)) FOR [WinCnt]
GO
ALTER TABLE [dbo].[tbl_history_electraceboss_candidacy_2] ADD  CONSTRAINT [DF_tbl_history_electraceboss_candidacy_2_bGiveback]  DEFAULT ((0)) FOR [bGiveback]
GO
ALTER TABLE [dbo].[tbl_history_electraceboss_candidacy_2] ADD  CONSTRAINT [DF_tbl_history_electraceboss_candidacy_2_bPrevBoss]  DEFAULT ((0)) FOR [bPrevBoss]
GO
ALTER TABLE [dbo].[tbl_history_electraceboss_candidacy_2] ADD  CONSTRAINT [DF_tbl_history_electraceboss_candidacy_2_bregSecond]  DEFAULT ((0)) FOR [bregSecond]
GO
ALTER TABLE [dbo].[tbl_history_electraceboss_candidacy_2] ADD  CONSTRAINT [DF_tbl_history_electraceboss_candidacy_2_tmReg_F]  DEFAULT (getdate()) FOR [tmReg_F]
GO
ALTER TABLE [dbo].[tbl_history_electraceboss_candidacy_2] ADD  CONSTRAINT [DF_tbl_history_electraceboss_candidacy_2_tmReg_S]  DEFAULT (getdate()) FOR [tmReg_S]
GO
ALTER TABLE [dbo].[tbl_history_electraceboss_candidacy_2] ADD  CONSTRAINT [DF_tbl_history_electraceboss_candidacy_2_GuildSerial]  DEFAULT ((-1)) FOR [GuildSerial]
GO
ALTER TABLE [dbo].[tbl_history_electraceboss_candidacy_2] ADD  CONSTRAINT [DF_tbl_history_electraceboss_candidacy_2_GuildName]  DEFAULT (N'(*)') FOR [GuildName]
GO
ALTER TABLE [dbo].[tbl_history_electraceboss_candidacy_2] ADD  CONSTRAINT [DF_tbl_history_electraceboss_candidacy_2_Rank]  DEFAULT ((0)) FOR [Rank]
GO
ALTER TABLE [dbo].[tbl_history_electraceboss_candidacy_2] ADD  CONSTRAINT [DF_tbl_history_electraceboss_candidacy_2_Score]  DEFAULT ((0)) FOR [Score]
GO
ALTER TABLE [dbo].[tbl_history_electraceboss_candidacy_2] ADD  CONSTRAINT [DF_tbl_history_electraceboss_candidacy_2_WinCnt]  DEFAULT ((0)) FOR [WinCnt]
GO
ALTER TABLE [dbo].[tbl_honor_guild] ADD  CONSTRAINT [DF__tbl_honor__Guild__3D94271D]  DEFAULT (0xFFFFFFFF) FOR [GuildSerial]
GO
ALTER TABLE [dbo].[tbl_honor_guild] ADD  CONSTRAINT [DF__tbl_honor__IsNex__3E884B56]  DEFAULT ((0)) FOR [IsNext]
GO
ALTER TABLE [dbo].[tbl_honor_guild] ADD  CONSTRAINT [DF__tbl_honor_g__DCK__3F7C6F8F]  DEFAULT ((0)) FOR [DCK]
GO
ALTER TABLE [dbo].[tbl_honor_guild] ADD  CONSTRAINT [DF__tbl_honor__dtUpd__407093C8]  DEFAULT (getdate()) FOR [dtUpdate]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF_tbl_inven_Serial]  DEFAULT ((-1)) FOR [Serial]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF_tbl_inven_DCK]  DEFAULT ((0)) FOR [DCK]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF_tbl_inven_K0]  DEFAULT (0xFFFFFFFF) FOR [K0]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF_tbl_inven_D0]  DEFAULT ((0)) FOR [D0]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF_tbl_inven_U0]  DEFAULT (0x0FFFFFFF) FOR [U0]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF_tbl_inven_K1]  DEFAULT (0xFFFFFFFF) FOR [K1]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF_tbl_inven_D1]  DEFAULT ((0)) FOR [D1]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF_tbl_inven_U1]  DEFAULT (0x0FFFFFFF) FOR [U1]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF_tbl_inven_K2]  DEFAULT (0xFFFFFFFF) FOR [K2]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF_tbl_inven_D2]  DEFAULT ((0)) FOR [D2]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF_tbl_inven_U2]  DEFAULT (0x0FFFFFFF) FOR [U2]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF_tbl_inven_K3]  DEFAULT (0xFFFFFFFF) FOR [K3]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF_tbl_inven_D3]  DEFAULT ((0)) FOR [D3]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF_tbl_inven_U3]  DEFAULT (0x0FFFFFFF) FOR [U3]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF_tbl_inven_K4]  DEFAULT (0xFFFFFFFF) FOR [K4]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF_tbl_inven_D4]  DEFAULT ((0)) FOR [D4]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF_tbl_inven_U4]  DEFAULT (0x0FFFFFFF) FOR [U4]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF_tbl_inven_K5]  DEFAULT (0xFFFFFFFF) FOR [K5]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF_tbl_inven_D5]  DEFAULT ((0)) FOR [D5]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF_tbl_inven_U5]  DEFAULT (0x0FFFFFFF) FOR [U5]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF_tbl_inven_K6]  DEFAULT (0xFFFFFFFF) FOR [K6]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF_tbl_inven_D6]  DEFAULT ((0)) FOR [D6]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF_tbl_inven_U6]  DEFAULT (0x0FFFFFFF) FOR [U6]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF_tbl_inven_K7]  DEFAULT (0xFFFFFFFF) FOR [K7]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF_tbl_inven_D7]  DEFAULT ((0)) FOR [D7]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF_tbl_inven_U7]  DEFAULT (0x0FFFFFFF) FOR [U7]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF_tbl_inven_K8]  DEFAULT (0xFFFFFFFF) FOR [K8]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF_tbl_inven_D8]  DEFAULT ((0)) FOR [D8]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF_tbl_inven_U8]  DEFAULT (0x0FFFFFFF) FOR [U8]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF_tbl_inven_K9]  DEFAULT (0xFFFFFFFF) FOR [K9]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF_tbl_inven_D9]  DEFAULT ((0)) FOR [D9]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF_tbl_inven_U9]  DEFAULT (0x0FFFFFFF) FOR [U9]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF_tbl_inven_K10]  DEFAULT (0xFFFFFFFF) FOR [K10]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF_tbl_inven_D10]  DEFAULT ((0)) FOR [D10]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF_tbl_inven_U10]  DEFAULT (0x0FFFFFFF) FOR [U10]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF_tbl_inven_K11]  DEFAULT (0xFFFFFFFF) FOR [K11]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF_tbl_inven_D11]  DEFAULT ((0)) FOR [D11]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF_tbl_inven_U11]  DEFAULT (0x0FFFFFFF) FOR [U11]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF_tbl_inven_K12]  DEFAULT (0xFFFFFFFF) FOR [K12]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF_tbl_inven_D12]  DEFAULT ((0)) FOR [D12]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF_tbl_inven_U12]  DEFAULT (0x0FFFFFFF) FOR [U12]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF_tbl_inven_K13]  DEFAULT (0xFFFFFFFF) FOR [K13]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF_tbl_inven_D13]  DEFAULT ((0)) FOR [D13]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF_tbl_inven_U13]  DEFAULT (0x0FFFFFFF) FOR [U13]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF_tbl_inven_K14]  DEFAULT (0xFFFFFFFF) FOR [K14]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF_tbl_inven_D14]  DEFAULT ((0)) FOR [D14]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF_tbl_inven_U14]  DEFAULT (0x0FFFFFFF) FOR [U14]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF_tbl_inven_K15]  DEFAULT (0xFFFFFFFF) FOR [K15]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF_tbl_inven_D15]  DEFAULT ((0)) FOR [D15]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF_tbl_inven_U15]  DEFAULT (0x0FFFFFFF) FOR [U15]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF_tbl_inven_K16]  DEFAULT (0xFFFFFFFF) FOR [K16]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF_tbl_inven_D16]  DEFAULT ((0)) FOR [D16]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF_tbl_inven_U16]  DEFAULT (0x0FFFFFFF) FOR [U16]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF_tbl_inven_K17]  DEFAULT (0xFFFFFFFF) FOR [K17]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF_tbl_inven_D17]  DEFAULT ((0)) FOR [D17]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF_tbl_inven_U17]  DEFAULT (0x0FFFFFFF) FOR [U17]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF_tbl_inven_K18]  DEFAULT (0xFFFFFFFF) FOR [K18]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF_tbl_inven_D18]  DEFAULT ((0)) FOR [D18]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF_tbl_inven_U18]  DEFAULT (0x0FFFFFFF) FOR [U18]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF_tbl_inven_K19]  DEFAULT (0xFFFFFFFF) FOR [K19]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF_tbl_inven_D19]  DEFAULT ((0)) FOR [D19]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF_tbl_inven_U19]  DEFAULT (0x0FFFFFFF) FOR [U19]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF_tbl_inven_K20]  DEFAULT (0xFFFFFFFF) FOR [K20]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF_tbl_inven_D20]  DEFAULT ((0)) FOR [D20]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF_tbl_inven_U20]  DEFAULT (0x0FFFFFFF) FOR [U20]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF_tbl_inven_K21]  DEFAULT (0xFFFFFFFF) FOR [K21]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF_tbl_inven_D21]  DEFAULT ((0)) FOR [D21]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF_tbl_inven_U21]  DEFAULT (0x0FFFFFFF) FOR [U21]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF_tbl_inven_K22]  DEFAULT (0xFFFFFFFF) FOR [K22]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF_tbl_inven_D22]  DEFAULT ((0)) FOR [D22]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF_tbl_inven_U22]  DEFAULT (0x0FFFFFFF) FOR [U22]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF_tbl_inven_K23]  DEFAULT (0xFFFFFFFF) FOR [K23]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF_tbl_inven_D23]  DEFAULT ((0)) FOR [D23]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF_tbl_inven_U23]  DEFAULT (0x0FFFFFFF) FOR [U23]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF_tbl_inven_K24]  DEFAULT (0xFFFFFFFF) FOR [K24]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF_tbl_inven_D24]  DEFAULT ((0)) FOR [D24]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF_tbl_inven_U24]  DEFAULT (0x0FFFFFFF) FOR [U24]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF_tbl_inven_K25]  DEFAULT (0xFFFFFFFF) FOR [K25]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF_tbl_inven_D25]  DEFAULT ((0)) FOR [D25]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF_tbl_inven_U25]  DEFAULT (0x0FFFFFFF) FOR [U25]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF_tbl_inven_K26]  DEFAULT (0xFFFFFFFF) FOR [K26]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF_tbl_inven_D26]  DEFAULT ((0)) FOR [D26]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF_tbl_inven_U26]  DEFAULT (0x0FFFFFFF) FOR [U26]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF_tbl_inven_K27]  DEFAULT (0xFFFFFFFF) FOR [K27]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF_tbl_inven_D27]  DEFAULT ((0)) FOR [D27]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF_tbl_inven_U27]  DEFAULT (0x0FFFFFFF) FOR [U27]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF_tbl_inven_K28]  DEFAULT (0xFFFFFFFF) FOR [K28]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF_tbl_inven_D28]  DEFAULT ((0)) FOR [D28]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF_tbl_inven_U28]  DEFAULT (0x0FFFFFFF) FOR [U28]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF_tbl_inven_K29]  DEFAULT (0xFFFFFFFF) FOR [K29]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF_tbl_inven_D29]  DEFAULT ((0)) FOR [D29]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF_tbl_inven_U29]  DEFAULT (0x0FFFFFFF) FOR [U29]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF_tbl_inven_K30]  DEFAULT (0xFFFFFFFF) FOR [K30]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF_tbl_inven_D30]  DEFAULT ((0)) FOR [D30]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF_tbl_inven_U30]  DEFAULT (0x0FFFFFFF) FOR [U30]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF_tbl_inven_K31]  DEFAULT (0xFFFFFFFF) FOR [K31]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF_tbl_inven_D31]  DEFAULT ((0)) FOR [D31]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF_tbl_inven_U31]  DEFAULT (0x0FFFFFFF) FOR [U31]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF_tbl_inven_K32]  DEFAULT (0xFFFFFFFF) FOR [K32]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF_tbl_inven_D32]  DEFAULT ((0)) FOR [D32]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF_tbl_inven_U32]  DEFAULT (0x0FFFFFFF) FOR [U32]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF_tbl_inven_K33]  DEFAULT (0xFFFFFFFF) FOR [K33]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF_tbl_inven_D33]  DEFAULT ((0)) FOR [D33]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF_tbl_inven_U33]  DEFAULT (0x0FFFFFFF) FOR [U33]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF_tbl_inven_K34]  DEFAULT (0xFFFFFFFF) FOR [K34]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF_tbl_inven_D34]  DEFAULT ((0)) FOR [D34]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF_tbl_inven_U34]  DEFAULT (0x0FFFFFFF) FOR [U34]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF_tbl_inven_K35]  DEFAULT (0xFFFFFFFF) FOR [K35]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF_tbl_inven_D35]  DEFAULT ((0)) FOR [D35]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF_tbl_inven_U35]  DEFAULT (0x0FFFFFFF) FOR [U35]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF_tbl_inven_K36]  DEFAULT (0xFFFFFFFF) FOR [K36]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF_tbl_inven_D36]  DEFAULT ((0)) FOR [D36]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF_tbl_inven_U36]  DEFAULT (0x0FFFFFFF) FOR [U36]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF_tbl_inven_K37]  DEFAULT (0xFFFFFFFF) FOR [K37]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF_tbl_inven_D37]  DEFAULT ((0)) FOR [D37]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF_tbl_inven_U37]  DEFAULT (0x0FFFFFFF) FOR [U37]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF_tbl_inven_K38]  DEFAULT (0xFFFFFFFF) FOR [K38]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF_tbl_inven_D38]  DEFAULT ((0)) FOR [D38]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF_tbl_inven_U38]  DEFAULT (0x0FFFFFFF) FOR [U38]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF_tbl_inven_K39]  DEFAULT (0xFFFFFFFF) FOR [K39]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF_tbl_inven_D39]  DEFAULT ((0)) FOR [D39]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF_tbl_inven_U39]  DEFAULT (0x0FFFFFFF) FOR [U39]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF_tbl_inven_K40]  DEFAULT (0xFFFFFFFF) FOR [K40]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF_tbl_inven_D40]  DEFAULT ((0)) FOR [D40]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF_tbl_inven_U40]  DEFAULT (0x0FFFFFFF) FOR [U40]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF_tbl_inven_K41]  DEFAULT (0xFFFFFFFF) FOR [K41]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF_tbl_inven_D41]  DEFAULT ((0)) FOR [D41]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF_tbl_inven_U41]  DEFAULT (0x0FFFFFFF) FOR [U41]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF_tbl_inven_K42]  DEFAULT (0xFFFFFFFF) FOR [K42]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF_tbl_inven_D42]  DEFAULT ((0)) FOR [D42]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF_tbl_inven_U42]  DEFAULT (0x0FFFFFFF) FOR [U42]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF_tbl_inven_K43]  DEFAULT (0xFFFFFFFF) FOR [K43]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF_tbl_inven_D43]  DEFAULT ((0)) FOR [D43]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF_tbl_inven_U43]  DEFAULT (0x0FFFFFFF) FOR [U43]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF_tbl_inven_K44]  DEFAULT (0xFFFFFFFF) FOR [K44]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF_tbl_inven_D44]  DEFAULT ((0)) FOR [D44]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF_tbl_inven_U44]  DEFAULT (0x0FFFFFFF) FOR [U44]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF_tbl_inven_K45]  DEFAULT (0xFFFFFFFF) FOR [K45]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF_tbl_inven_D45]  DEFAULT ((0)) FOR [D45]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF_tbl_inven_U45]  DEFAULT (0x0FFFFFFF) FOR [U45]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF_tbl_inven_K46]  DEFAULT (0xFFFFFFFF) FOR [K46]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF_tbl_inven_D46]  DEFAULT ((0)) FOR [D46]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF_tbl_inven_U46]  DEFAULT (0x0FFFFFFF) FOR [U46]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF_tbl_inven_K47]  DEFAULT (0xFFFFFFFF) FOR [K47]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF_tbl_inven_D47]  DEFAULT ((0)) FOR [D47]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF_tbl_inven_U47]  DEFAULT (0x0FFFFFFF) FOR [U47]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF_tbl_inven_K48]  DEFAULT (0xFFFFFFFF) FOR [K48]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF_tbl_inven_D48]  DEFAULT ((0)) FOR [D48]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF_tbl_inven_U48]  DEFAULT (0x0FFFFFFF) FOR [U48]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF_tbl_inven_K49]  DEFAULT (0xFFFFFFFF) FOR [K49]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF_tbl_inven_D49]  DEFAULT ((0)) FOR [D49]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF_tbl_inven_U49]  DEFAULT (0x0FFFFFFF) FOR [U49]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF_tbl_inven_K50]  DEFAULT (0xFFFFFFFF) FOR [K50]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF_tbl_inven_D50]  DEFAULT ((0)) FOR [D50]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF_tbl_inven_U50]  DEFAULT (0x0FFFFFFF) FOR [U50]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF_tbl_inven_K51]  DEFAULT (0xFFFFFFFF) FOR [K51]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF_tbl_inven_D51]  DEFAULT ((0)) FOR [D51]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF_tbl_inven_U51]  DEFAULT (0x0FFFFFFF) FOR [U51]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF_tbl_inven_K52]  DEFAULT (0xFFFFFFFF) FOR [K52]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF_tbl_inven_D52]  DEFAULT ((0)) FOR [D52]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF_tbl_inven_U52]  DEFAULT (0x0FFFFFFF) FOR [U52]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF_tbl_inven_K53]  DEFAULT (0xFFFFFFFF) FOR [K53]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF_tbl_inven_D53]  DEFAULT ((0)) FOR [D53]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF_tbl_inven_U53]  DEFAULT (0x0FFFFFFF) FOR [U53]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF_tbl_inven_K54]  DEFAULT (0xFFFFFFFF) FOR [K54]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF_tbl_inven_D54]  DEFAULT ((0)) FOR [D54]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF_tbl_inven_U54]  DEFAULT (0x0FFFFFFF) FOR [U54]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF_tbl_inven_K55]  DEFAULT (0xFFFFFFFF) FOR [K55]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF_tbl_inven_D55]  DEFAULT ((0)) FOR [D55]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF_tbl_inven_U55]  DEFAULT (0x0FFFFFFF) FOR [U55]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF_tbl_inven_K56]  DEFAULT (0xFFFFFFFF) FOR [K56]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF_tbl_inven_D56]  DEFAULT ((0)) FOR [D56]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF_tbl_inven_U56]  DEFAULT (0x0FFFFFFF) FOR [U56]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF_tbl_inven_K57]  DEFAULT (0xFFFFFFFF) FOR [K57]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF_tbl_inven_D57]  DEFAULT ((0)) FOR [D57]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF_tbl_inven_U57]  DEFAULT (0x0FFFFFFF) FOR [U57]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF_tbl_inven_K58]  DEFAULT (0xFFFFFFFF) FOR [K58]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF_tbl_inven_D58]  DEFAULT ((0)) FOR [D58]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF_tbl_inven_U58]  DEFAULT (0x0FFFFFFF) FOR [U58]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF_tbl_inven_K59]  DEFAULT (0xFFFFFFFF) FOR [K59]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF_tbl_inven_D59]  DEFAULT ((0)) FOR [D59]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF_tbl_inven_U59]  DEFAULT (0x0FFFFFFF) FOR [U59]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF_tbl_inven_K60]  DEFAULT (0xFFFFFFFF) FOR [K60]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF_tbl_inven_D60]  DEFAULT ((0)) FOR [D60]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF_tbl_inven_U60]  DEFAULT (0x0FFFFFFF) FOR [U60]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF_tbl_inven_K61]  DEFAULT (0xFFFFFFFF) FOR [K61]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF_tbl_inven_D61]  DEFAULT ((0)) FOR [D61]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF_tbl_inven_U61]  DEFAULT (0x0FFFFFFF) FOR [U61]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF_tbl_inven_K62]  DEFAULT (0xFFFFFFFF) FOR [K62]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF_tbl_inven_D62]  DEFAULT ((0)) FOR [D62]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF_tbl_inven_U62]  DEFAULT (0x0FFFFFFF) FOR [U62]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF_tbl_inven_K63]  DEFAULT (0xFFFFFFFF) FOR [K63]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF_tbl_inven_D63]  DEFAULT ((0)) FOR [D63]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF_tbl_inven_U63]  DEFAULT (0x0FFFFFFF) FOR [U63]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF_tbl_inven_K64]  DEFAULT (0xFFFFFFFF) FOR [K64]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF_tbl_inven_D64]  DEFAULT ((0)) FOR [D64]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF_tbl_inven_U64]  DEFAULT (0x0FFFFFFF) FOR [U64]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF_tbl_inven_K65]  DEFAULT (0xFFFFFFFF) FOR [K65]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF_tbl_inven_D65]  DEFAULT ((0)) FOR [D65]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF_tbl_inven_U65]  DEFAULT (0x0FFFFFFF) FOR [U65]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF_tbl_inven_K66]  DEFAULT (0xFFFFFFFF) FOR [K66]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF_tbl_inven_D66]  DEFAULT ((0)) FOR [D66]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF_tbl_inven_U66]  DEFAULT (0x0FFFFFFF) FOR [U66]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF_tbl_inven_K67]  DEFAULT (0xFFFFFFFF) FOR [K67]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF_tbl_inven_D67]  DEFAULT ((0)) FOR [D67]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF_tbl_inven_U67]  DEFAULT (0x0FFFFFFF) FOR [U67]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF_tbl_inven_K68]  DEFAULT (0xFFFFFFFF) FOR [K68]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF_tbl_inven_D68]  DEFAULT ((0)) FOR [D68]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF_tbl_inven_U68]  DEFAULT (0x0FFFFFFF) FOR [U68]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF_tbl_inven_K69]  DEFAULT (0xFFFFFFFF) FOR [K69]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF_tbl_inven_D69]  DEFAULT ((0)) FOR [D69]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF_tbl_inven_U69]  DEFAULT (0x0FFFFFFF) FOR [U69]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF_tbl_inven_K70]  DEFAULT (0xFFFFFFFF) FOR [K70]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF_tbl_inven_D70]  DEFAULT ((0)) FOR [D70]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF_tbl_inven_U70]  DEFAULT (0x0FFFFFFF) FOR [U70]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF_tbl_inven_K71]  DEFAULT (0xFFFFFFFF) FOR [K71]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF_tbl_inven_D71]  DEFAULT ((0)) FOR [D71]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF_tbl_inven_U71]  DEFAULT (0x0FFFFFFF) FOR [U71]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF_tbl_inven_K72]  DEFAULT (0xFFFFFFFF) FOR [K72]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF_tbl_inven_D72]  DEFAULT ((0)) FOR [D72]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF_tbl_inven_U72]  DEFAULT (0x0FFFFFFF) FOR [U72]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF_tbl_inven_K73]  DEFAULT (0xFFFFFFFF) FOR [K73]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF_tbl_inven_D73]  DEFAULT ((0)) FOR [D73]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF_tbl_inven_U73]  DEFAULT (0x0FFFFFFF) FOR [U73]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF_tbl_inven_K74]  DEFAULT (0xFFFFFFFF) FOR [K74]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF_tbl_inven_D74]  DEFAULT ((0)) FOR [D74]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF_tbl_inven_U74]  DEFAULT (0x0FFFFFFF) FOR [U74]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF_tbl_inven_K75]  DEFAULT (0xFFFFFFFF) FOR [K75]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF_tbl_inven_D75]  DEFAULT ((0)) FOR [D75]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF_tbl_inven_U75]  DEFAULT (0x0FFFFFFF) FOR [U75]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF_tbl_inven_K76]  DEFAULT (0xFFFFFFFF) FOR [K76]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF_tbl_inven_D76]  DEFAULT ((0)) FOR [D76]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF_tbl_inven_U76]  DEFAULT (0x0FFFFFFF) FOR [U76]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF_tbl_inven_K77]  DEFAULT (0xFFFFFFFF) FOR [K77]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF_tbl_inven_D77]  DEFAULT ((0)) FOR [D77]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF_tbl_inven_U77]  DEFAULT (0x0FFFFFFF) FOR [U77]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF_tbl_inven_K78]  DEFAULT (0xFFFFFFFF) FOR [K78]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF_tbl_inven_D78]  DEFAULT ((0)) FOR [D78]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF_tbl_inven_U78]  DEFAULT (0x0FFFFFFF) FOR [U78]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF_tbl_inven_K79]  DEFAULT (0xFFFFFFFF) FOR [K79]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF_tbl_inven_D79]  DEFAULT ((0)) FOR [D79]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF_tbl_inven_U79]  DEFAULT (0x0FFFFFFF) FOR [U79]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF_tbl_inven_K80]  DEFAULT (0xFFFFFFFF) FOR [K80]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF_tbl_inven_D80]  DEFAULT ((0)) FOR [D80]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF_tbl_inven_U80]  DEFAULT (0x0FFFFFFF) FOR [U80]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF_tbl_inven_K81]  DEFAULT (0xFFFFFFFF) FOR [K81]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF_tbl_inven_D81]  DEFAULT ((0)) FOR [D81]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF_tbl_inven_U81]  DEFAULT (0x0FFFFFFF) FOR [U81]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF_tbl_inven_K82]  DEFAULT (0xFFFFFFFF) FOR [K82]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF_tbl_inven_D82]  DEFAULT ((0)) FOR [D82]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF_tbl_inven_U82]  DEFAULT (0x0FFFFFFF) FOR [U82]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF_tbl_inven_K83]  DEFAULT (0xFFFFFFFF) FOR [K83]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF_tbl_inven_D83]  DEFAULT ((0)) FOR [D83]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF_tbl_inven_U83]  DEFAULT (0x0FFFFFFF) FOR [U83]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF_tbl_inven_K84]  DEFAULT (0xFFFFFFFF) FOR [K84]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF_tbl_inven_D84]  DEFAULT ((0)) FOR [D84]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF_tbl_inven_U84]  DEFAULT (0x0FFFFFFF) FOR [U84]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF_tbl_inven_K85]  DEFAULT (0xFFFFFFFF) FOR [K85]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF_tbl_inven_D85]  DEFAULT ((0)) FOR [D85]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF_tbl_inven_U85]  DEFAULT (0x0FFFFFFF) FOR [U85]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF_tbl_inven_K86]  DEFAULT (0xFFFFFFFF) FOR [K86]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF_tbl_inven_D86]  DEFAULT ((0)) FOR [D86]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF_tbl_inven_U86]  DEFAULT (0x0FFFFFFF) FOR [U86]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF_tbl_inven_K87]  DEFAULT (0xFFFFFFFF) FOR [K87]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF_tbl_inven_D87]  DEFAULT ((0)) FOR [D87]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF_tbl_inven_U87]  DEFAULT (0x0FFFFFFF) FOR [U87]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF_tbl_inven_K88]  DEFAULT (0xFFFFFFFF) FOR [K88]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF_tbl_inven_D88]  DEFAULT ((0)) FOR [D88]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF_tbl_inven_U88]  DEFAULT (0x0FFFFFFF) FOR [U88]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF_tbl_inven_K89]  DEFAULT (0xFFFFFFFF) FOR [K89]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF_tbl_inven_D89]  DEFAULT ((0)) FOR [D89]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF_tbl_inven_U89]  DEFAULT (0x0FFFFFFF) FOR [U89]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF_tbl_inven_K90]  DEFAULT (0xFFFFFFFF) FOR [K90]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF_tbl_inven_D90]  DEFAULT ((0)) FOR [D90]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF_tbl_inven_U90]  DEFAULT (0x0FFFFFFF) FOR [U90]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF_tbl_inven_K91]  DEFAULT (0xFFFFFFFF) FOR [K91]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF_tbl_inven_D91]  DEFAULT ((0)) FOR [D91]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF_tbl_inven_U91]  DEFAULT (0x0FFFFFFF) FOR [U91]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF_tbl_inven_K92]  DEFAULT (0xFFFFFFFF) FOR [K92]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF_tbl_inven_D92]  DEFAULT ((0)) FOR [D92]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF_tbl_inven_U92]  DEFAULT (0x0FFFFFFF) FOR [U92]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF_tbl_inven_K93]  DEFAULT (0xFFFFFFFF) FOR [K93]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF_tbl_inven_D93]  DEFAULT ((0)) FOR [D93]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF_tbl_inven_U93]  DEFAULT (0x0FFFFFFF) FOR [U93]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF_tbl_inven_K94]  DEFAULT (0xFFFFFFFF) FOR [K94]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF_tbl_inven_D94]  DEFAULT ((0)) FOR [D94]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF_tbl_inven_U94]  DEFAULT (0x0FFFFFFF) FOR [U94]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF_tbl_inven_K95]  DEFAULT (0xFFFFFFFF) FOR [K95]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF_tbl_inven_D95]  DEFAULT ((0)) FOR [D95]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF_tbl_inven_U95]  DEFAULT (0x0FFFFFFF) FOR [U95]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF_tbl_inven_K96]  DEFAULT (0xFFFFFFFF) FOR [K96]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF_tbl_inven_D96]  DEFAULT ((0)) FOR [D96]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF_tbl_inven_U96]  DEFAULT (0x0FFFFFFF) FOR [U96]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF_tbl_inven_K97]  DEFAULT (0xFFFFFFFF) FOR [K97]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF_tbl_inven_D97]  DEFAULT ((0)) FOR [D97]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF_tbl_inven_U97]  DEFAULT (0x0FFFFFFF) FOR [U97]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF_tbl_inven_K98]  DEFAULT (0xFFFFFFFF) FOR [K98]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF_tbl_inven_D98]  DEFAULT ((0)) FOR [D98]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF_tbl_inven_U98]  DEFAULT (0x0FFFFFFF) FOR [U98]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF_tbl_inven_K99]  DEFAULT (0xFFFFFFFF) FOR [K99]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF_tbl_inven_D99]  DEFAULT ((0)) FOR [D99]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF_tbl_inven_U99]  DEFAULT (0x0FFFFFFF) FOR [U99]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF__tbl_inven__TrInd__56A9BBE0]  DEFAULT (0xFF) FOR [TrIndex0]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF__tbl_inven__TrPri__579DE019]  DEFAULT (0xFFFFFFFF) FOR [TrPrice0]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF__tbl_inven__TrPas__58920452]  DEFAULT ((0)) FOR [TrPass0]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF__tbl_inven__TrInd__5986288B]  DEFAULT (0xFF) FOR [TrIndex1]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF__tbl_inven__TrPri__5A7A4CC4]  DEFAULT (0xFFFFFFFF) FOR [TrPrice1]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF__tbl_inven__TrPas__5B6E70FD]  DEFAULT ((0)) FOR [TrPass1]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF__tbl_inven__TrInd__5C629536]  DEFAULT (0xFF) FOR [TrIndex2]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF__tbl_inven__TrPri__5D56B96F]  DEFAULT (0xFFFFFFFF) FOR [TrPrice2]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF__tbl_inven__TrPas__5E4ADDA8]  DEFAULT ((0)) FOR [TrPass2]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF__tbl_inven__TrInd__5F3F01E1]  DEFAULT (0xFF) FOR [TrIndex3]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF__tbl_inven__TrPri__6033261A]  DEFAULT (0xFFFFFFFF) FOR [TrPrice3]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF__tbl_inven__TrPas__61274A53]  DEFAULT ((0)) FOR [TrPass3]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF__tbl_inven__TrInd__621B6E8C]  DEFAULT (0xFF) FOR [TrIndex4]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF__tbl_inven__TrPri__630F92C5]  DEFAULT (0xFFFFFFFF) FOR [TrPrice4]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF__tbl_inven__TrPas__6403B6FE]  DEFAULT ((0)) FOR [TrPass4]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF__tbl_inven__TrInd__64F7DB37]  DEFAULT (0xFF) FOR [TrIndex5]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF__tbl_inven__TrPri__65EBFF70]  DEFAULT (0xFFFFFFFF) FOR [TrPrice5]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF__tbl_inven__TrPas__66E023A9]  DEFAULT ((0)) FOR [TrPass5]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF__tbl_inven__TrInd__67D447E2]  DEFAULT (0xFF) FOR [TrIndex6]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF__tbl_inven__TrPri__68C86C1B]  DEFAULT (0xFFFFFFFF) FOR [TrPrice6]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF__tbl_inven__TrPas__69BC9054]  DEFAULT ((0)) FOR [TrPass6]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF__tbl_inven__TrInd__6AB0B48D]  DEFAULT (0xFF) FOR [TrIndex7]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF__tbl_inven__TrPri__6BA4D8C6]  DEFAULT (0xFFFFFFFF) FOR [TrPrice7]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF__tbl_inven__TrPas__6C98FCFF]  DEFAULT ((0)) FOR [TrPass7]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF__tbl_inven__TrInd__6D8D2138]  DEFAULT (0xFF) FOR [TrIndex8]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF__tbl_inven__TrPri__6E814571]  DEFAULT (0xFFFFFFFF) FOR [TrPrice8]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF__tbl_inven__TrPas__6F7569AA]  DEFAULT ((0)) FOR [TrPass8]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF__tbl_inven__TrInd__70698DE3]  DEFAULT (0xFF) FOR [TrIndex9]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF__tbl_inven__TrPri__715DB21C]  DEFAULT (0xFFFFFFFF) FOR [TrPrice9]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF__tbl_inven__TrPas__7251D655]  DEFAULT ((0)) FOR [TrPass9]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF__tbl_inven__S0__76C9771D]  DEFAULT ((0)) FOR [S0]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF__tbl_inven__T0__77BD9B56]  DEFAULT (0xFFFFFFFF) FOR [T0]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF__tbl_inven__S1__78B1BF8F]  DEFAULT ((0)) FOR [S1]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF__tbl_inven__T1__79A5E3C8]  DEFAULT (0xFFFFFFFF) FOR [T1]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF__tbl_inven__S2__7A9A0801]  DEFAULT ((0)) FOR [S2]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF__tbl_inven__T2__7B8E2C3A]  DEFAULT (0xFFFFFFFF) FOR [T2]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF__tbl_inven__S3__7C825073]  DEFAULT ((0)) FOR [S3]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF__tbl_inven__T3__7D7674AC]  DEFAULT (0xFFFFFFFF) FOR [T3]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF__tbl_inven__S4__7E6A98E5]  DEFAULT ((0)) FOR [S4]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF__tbl_inven__T4__7F5EBD1E]  DEFAULT (0xFFFFFFFF) FOR [T4]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF__tbl_inven__S5__0052E157]  DEFAULT ((0)) FOR [S5]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF__tbl_inven__T5__01470590]  DEFAULT (0xFFFFFFFF) FOR [T5]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF__tbl_inven__S6__023B29C9]  DEFAULT ((0)) FOR [S6]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF__tbl_inven__T6__032F4E02]  DEFAULT (0xFFFFFFFF) FOR [T6]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF__tbl_inven__S7__0423723B]  DEFAULT ((0)) FOR [S7]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF__tbl_inven__T7__05179674]  DEFAULT (0xFFFFFFFF) FOR [T7]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF__tbl_inven__S8__060BBAAD]  DEFAULT ((0)) FOR [S8]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF__tbl_inven__T8__06FFDEE6]  DEFAULT (0xFFFFFFFF) FOR [T8]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF__tbl_inven__S9__07F4031F]  DEFAULT ((0)) FOR [S9]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF__tbl_inven__T9__08E82758]  DEFAULT (0xFFFFFFFF) FOR [T9]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF__tbl_inven__S10__09DC4B91]  DEFAULT ((0)) FOR [S10]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF__tbl_inven__T10__0AD06FCA]  DEFAULT (0xFFFFFFFF) FOR [T10]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF__tbl_inven__S11__0BC49403]  DEFAULT ((0)) FOR [S11]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF__tbl_inven__T11__0CB8B83C]  DEFAULT (0xFFFFFFFF) FOR [T11]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF__tbl_inven__S12__0DACDC75]  DEFAULT ((0)) FOR [S12]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF__tbl_inven__T12__0EA100AE]  DEFAULT (0xFFFFFFFF) FOR [T12]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF__tbl_inven__S13__0F9524E7]  DEFAULT ((0)) FOR [S13]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF__tbl_inven__T13__10894920]  DEFAULT (0xFFFFFFFF) FOR [T13]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF__tbl_inven__S14__117D6D59]  DEFAULT ((0)) FOR [S14]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF__tbl_inven__T14__12719192]  DEFAULT (0xFFFFFFFF) FOR [T14]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF__tbl_inven__S15__1365B5CB]  DEFAULT ((0)) FOR [S15]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF__tbl_inven__T15__1459DA04]  DEFAULT (0xFFFFFFFF) FOR [T15]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF__tbl_inven__S16__154DFE3D]  DEFAULT ((0)) FOR [S16]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF__tbl_inven__T16__16422276]  DEFAULT (0xFFFFFFFF) FOR [T16]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF__tbl_inven__S17__173646AF]  DEFAULT ((0)) FOR [S17]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF__tbl_inven__T17__182A6AE8]  DEFAULT (0xFFFFFFFF) FOR [T17]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF__tbl_inven__S18__191E8F21]  DEFAULT ((0)) FOR [S18]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF__tbl_inven__T18__1A12B35A]  DEFAULT (0xFFFFFFFF) FOR [T18]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF__tbl_inven__S19__1B06D793]  DEFAULT ((0)) FOR [S19]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF__tbl_inven__T19__1BFAFBCC]  DEFAULT (0xFFFFFFFF) FOR [T19]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF__tbl_inven__S20__1CEF2005]  DEFAULT ((0)) FOR [S20]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF__tbl_inven__T20__1DE3443E]  DEFAULT (0xFFFFFFFF) FOR [T20]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF__tbl_inven__S21__1ED76877]  DEFAULT ((0)) FOR [S21]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF__tbl_inven__T21__1FCB8CB0]  DEFAULT (0xFFFFFFFF) FOR [T21]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF__tbl_inven__S22__20BFB0E9]  DEFAULT ((0)) FOR [S22]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF__tbl_inven__T22__21B3D522]  DEFAULT (0xFFFFFFFF) FOR [T22]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF__tbl_inven__S23__22A7F95B]  DEFAULT ((0)) FOR [S23]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF__tbl_inven__T23__239C1D94]  DEFAULT (0xFFFFFFFF) FOR [T23]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF__tbl_inven__S24__249041CD]  DEFAULT ((0)) FOR [S24]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF__tbl_inven__T24__25846606]  DEFAULT (0xFFFFFFFF) FOR [T24]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF__tbl_inven__S25__26788A3F]  DEFAULT ((0)) FOR [S25]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF__tbl_inven__T25__276CAE78]  DEFAULT (0xFFFFFFFF) FOR [T25]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF__tbl_inven__S26__2860D2B1]  DEFAULT ((0)) FOR [S26]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF__tbl_inven__T26__2954F6EA]  DEFAULT (0xFFFFFFFF) FOR [T26]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF__tbl_inven__S27__2A491B23]  DEFAULT ((0)) FOR [S27]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF__tbl_inven__T27__2B3D3F5C]  DEFAULT (0xFFFFFFFF) FOR [T27]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF__tbl_inven__S28__2C316395]  DEFAULT ((0)) FOR [S28]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF__tbl_inven__T28__2D2587CE]  DEFAULT (0xFFFFFFFF) FOR [T28]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF__tbl_inven__S29__2E19AC07]  DEFAULT ((0)) FOR [S29]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF__tbl_inven__T29__2F0DD040]  DEFAULT (0xFFFFFFFF) FOR [T29]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF__tbl_inven__S30__3001F479]  DEFAULT ((0)) FOR [S30]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF__tbl_inven__T30__30F618B2]  DEFAULT (0xFFFFFFFF) FOR [T30]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF__tbl_inven__S31__31EA3CEB]  DEFAULT ((0)) FOR [S31]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF__tbl_inven__T31__32DE6124]  DEFAULT (0xFFFFFFFF) FOR [T31]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF__tbl_inven__S32__33D2855D]  DEFAULT ((0)) FOR [S32]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF__tbl_inven__T32__34C6A996]  DEFAULT (0xFFFFFFFF) FOR [T32]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF__tbl_inven__S33__35BACDCF]  DEFAULT ((0)) FOR [S33]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF__tbl_inven__T33__36AEF208]  DEFAULT (0xFFFFFFFF) FOR [T33]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF__tbl_inven__S34__37A31641]  DEFAULT ((0)) FOR [S34]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF__tbl_inven__T34__38973A7A]  DEFAULT (0xFFFFFFFF) FOR [T34]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF__tbl_inven__S35__398B5EB3]  DEFAULT ((0)) FOR [S35]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF__tbl_inven__T35__3A7F82EC]  DEFAULT (0xFFFFFFFF) FOR [T35]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF__tbl_inven__S36__3B73A725]  DEFAULT ((0)) FOR [S36]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF__tbl_inven__T36__3C67CB5E]  DEFAULT (0xFFFFFFFF) FOR [T36]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF__tbl_inven__S37__3D5BEF97]  DEFAULT ((0)) FOR [S37]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF__tbl_inven__T37__3E5013D0]  DEFAULT (0xFFFFFFFF) FOR [T37]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF__tbl_inven__S38__3F443809]  DEFAULT ((0)) FOR [S38]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF__tbl_inven__T38__40385C42]  DEFAULT (0xFFFFFFFF) FOR [T38]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF__tbl_inven__S39__412C807B]  DEFAULT ((0)) FOR [S39]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF__tbl_inven__T39__4220A4B4]  DEFAULT (0xFFFFFFFF) FOR [T39]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF__tbl_inven__S40__4314C8ED]  DEFAULT ((0)) FOR [S40]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF__tbl_inven__T40__4408ED26]  DEFAULT (0xFFFFFFFF) FOR [T40]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF__tbl_inven__S41__44FD115F]  DEFAULT ((0)) FOR [S41]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF__tbl_inven__T41__45F13598]  DEFAULT (0xFFFFFFFF) FOR [T41]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF__tbl_inven__S42__46E559D1]  DEFAULT ((0)) FOR [S42]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF__tbl_inven__T42__47D97E0A]  DEFAULT (0xFFFFFFFF) FOR [T42]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF__tbl_inven__S43__48CDA243]  DEFAULT ((0)) FOR [S43]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF__tbl_inven__T43__49C1C67C]  DEFAULT (0xFFFFFFFF) FOR [T43]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF__tbl_inven__S44__4AB5EAB5]  DEFAULT ((0)) FOR [S44]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF__tbl_inven__T44__4BAA0EEE]  DEFAULT (0xFFFFFFFF) FOR [T44]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF__tbl_inven__S45__4C9E3327]  DEFAULT ((0)) FOR [S45]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF__tbl_inven__T45__4D925760]  DEFAULT (0xFFFFFFFF) FOR [T45]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF__tbl_inven__S46__4E867B99]  DEFAULT ((0)) FOR [S46]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF__tbl_inven__T46__4F7A9FD2]  DEFAULT (0xFFFFFFFF) FOR [T46]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF__tbl_inven__S47__506EC40B]  DEFAULT ((0)) FOR [S47]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF__tbl_inven__T47__5162E844]  DEFAULT (0xFFFFFFFF) FOR [T47]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF__tbl_inven__S48__52570C7D]  DEFAULT ((0)) FOR [S48]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF__tbl_inven__T48__534B30B6]  DEFAULT (0xFFFFFFFF) FOR [T48]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF__tbl_inven__S49__543F54EF]  DEFAULT ((0)) FOR [S49]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF__tbl_inven__T49__55337928]  DEFAULT (0xFFFFFFFF) FOR [T49]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF__tbl_inven__S50__56279D61]  DEFAULT ((0)) FOR [S50]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF__tbl_inven__T50__571BC19A]  DEFAULT (0xFFFFFFFF) FOR [T50]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF__tbl_inven__S51__580FE5D3]  DEFAULT ((0)) FOR [S51]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF__tbl_inven__T51__59040A0C]  DEFAULT (0xFFFFFFFF) FOR [T51]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF__tbl_inven__S52__59F82E45]  DEFAULT ((0)) FOR [S52]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF__tbl_inven__T52__5AEC527E]  DEFAULT (0xFFFFFFFF) FOR [T52]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF__tbl_inven__S53__5BE076B7]  DEFAULT ((0)) FOR [S53]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF__tbl_inven__T53__5CD49AF0]  DEFAULT (0xFFFFFFFF) FOR [T53]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF__tbl_inven__S54__5DC8BF29]  DEFAULT ((0)) FOR [S54]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF__tbl_inven__T54__5EBCE362]  DEFAULT (0xFFFFFFFF) FOR [T54]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF__tbl_inven__S55__5FB1079B]  DEFAULT ((0)) FOR [S55]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF__tbl_inven__T55__60A52BD4]  DEFAULT (0xFFFFFFFF) FOR [T55]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF__tbl_inven__S56__6199500D]  DEFAULT ((0)) FOR [S56]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF__tbl_inven__T56__628D7446]  DEFAULT (0xFFFFFFFF) FOR [T56]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF__tbl_inven__S57__6381987F]  DEFAULT ((0)) FOR [S57]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF__tbl_inven__T57__6475BCB8]  DEFAULT (0xFFFFFFFF) FOR [T57]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF__tbl_inven__S58__6569E0F1]  DEFAULT ((0)) FOR [S58]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF__tbl_inven__T58__665E052A]  DEFAULT (0xFFFFFFFF) FOR [T58]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF__tbl_inven__S59__67522963]  DEFAULT ((0)) FOR [S59]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF__tbl_inven__T59__68464D9C]  DEFAULT (0xFFFFFFFF) FOR [T59]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF__tbl_inven__S60__693A71D5]  DEFAULT ((0)) FOR [S60]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF__tbl_inven__T60__6A2E960E]  DEFAULT (0xFFFFFFFF) FOR [T60]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF__tbl_inven__S61__6B22BA47]  DEFAULT ((0)) FOR [S61]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF__tbl_inven__T61__6C16DE80]  DEFAULT (0xFFFFFFFF) FOR [T61]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF__tbl_inven__S62__6D0B02B9]  DEFAULT ((0)) FOR [S62]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF__tbl_inven__T62__6DFF26F2]  DEFAULT (0xFFFFFFFF) FOR [T62]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF__tbl_inven__S63__6EF34B2B]  DEFAULT ((0)) FOR [S63]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF__tbl_inven__T63__6FE76F64]  DEFAULT (0xFFFFFFFF) FOR [T63]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF__tbl_inven__S64__70DB939D]  DEFAULT ((0)) FOR [S64]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF__tbl_inven__T64__71CFB7D6]  DEFAULT (0xFFFFFFFF) FOR [T64]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF__tbl_inven__S65__72C3DC0F]  DEFAULT ((0)) FOR [S65]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF__tbl_inven__T65__73B80048]  DEFAULT (0xFFFFFFFF) FOR [T65]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF__tbl_inven__S66__74AC2481]  DEFAULT ((0)) FOR [S66]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF__tbl_inven__T66__75A048BA]  DEFAULT (0xFFFFFFFF) FOR [T66]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF__tbl_inven__S67__76946CF3]  DEFAULT ((0)) FOR [S67]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF__tbl_inven__T67__7788912C]  DEFAULT (0xFFFFFFFF) FOR [T67]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF__tbl_inven__S68__787CB565]  DEFAULT ((0)) FOR [S68]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF__tbl_inven__T68__7970D99E]  DEFAULT (0xFFFFFFFF) FOR [T68]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF__tbl_inven__S69__7A64FDD7]  DEFAULT ((0)) FOR [S69]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF__tbl_inven__T69__7B592210]  DEFAULT (0xFFFFFFFF) FOR [T69]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF__tbl_inven__S70__7C4D4649]  DEFAULT ((0)) FOR [S70]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF__tbl_inven__T70__7D416A82]  DEFAULT (0xFFFFFFFF) FOR [T70]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF__tbl_inven__S71__7E358EBB]  DEFAULT ((0)) FOR [S71]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF__tbl_inven__T71__7F29B2F4]  DEFAULT (0xFFFFFFFF) FOR [T71]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF__tbl_inven__S72__001DD72D]  DEFAULT ((0)) FOR [S72]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF__tbl_inven__T72__0111FB66]  DEFAULT (0xFFFFFFFF) FOR [T72]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF__tbl_inven__S73__02061F9F]  DEFAULT ((0)) FOR [S73]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF__tbl_inven__T73__02FA43D8]  DEFAULT (0xFFFFFFFF) FOR [T73]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF__tbl_inven__S74__03EE6811]  DEFAULT ((0)) FOR [S74]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF__tbl_inven__T74__04E28C4A]  DEFAULT (0xFFFFFFFF) FOR [T74]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF__tbl_inven__S75__05D6B083]  DEFAULT ((0)) FOR [S75]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF__tbl_inven__T75__06CAD4BC]  DEFAULT (0xFFFFFFFF) FOR [T75]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF__tbl_inven__S76__07BEF8F5]  DEFAULT ((0)) FOR [S76]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF__tbl_inven__T76__08B31D2E]  DEFAULT (0xFFFFFFFF) FOR [T76]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF__tbl_inven__S77__09A74167]  DEFAULT ((0)) FOR [S77]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF__tbl_inven__T77__0A9B65A0]  DEFAULT (0xFFFFFFFF) FOR [T77]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF__tbl_inven__S78__0B8F89D9]  DEFAULT ((0)) FOR [S78]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF__tbl_inven__T78__0C83AE12]  DEFAULT (0xFFFFFFFF) FOR [T78]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF__tbl_inven__S79__0D77D24B]  DEFAULT ((0)) FOR [S79]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF__tbl_inven__T79__0E6BF684]  DEFAULT (0xFFFFFFFF) FOR [T79]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF__tbl_inven__S80__0F601ABD]  DEFAULT ((0)) FOR [S80]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF__tbl_inven__T80__10543EF6]  DEFAULT (0xFFFFFFFF) FOR [T80]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF__tbl_inven__S81__1148632F]  DEFAULT ((0)) FOR [S81]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF__tbl_inven__T81__123C8768]  DEFAULT (0xFFFFFFFF) FOR [T81]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF__tbl_inven__S82__1330ABA1]  DEFAULT ((0)) FOR [S82]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF__tbl_inven__T82__1424CFDA]  DEFAULT (0xFFFFFFFF) FOR [T82]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF__tbl_inven__S83__1518F413]  DEFAULT ((0)) FOR [S83]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF__tbl_inven__T83__160D184C]  DEFAULT (0xFFFFFFFF) FOR [T83]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF__tbl_inven__S84__17013C85]  DEFAULT ((0)) FOR [S84]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF__tbl_inven__T84__17F560BE]  DEFAULT (0xFFFFFFFF) FOR [T84]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF__tbl_inven__S85__18E984F7]  DEFAULT ((0)) FOR [S85]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF__tbl_inven__T85__19DDA930]  DEFAULT (0xFFFFFFFF) FOR [T85]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF__tbl_inven__S86__1AD1CD69]  DEFAULT ((0)) FOR [S86]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF__tbl_inven__T86__1BC5F1A2]  DEFAULT (0xFFFFFFFF) FOR [T86]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF__tbl_inven__S87__1CBA15DB]  DEFAULT ((0)) FOR [S87]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF__tbl_inven__T87__1DAE3A14]  DEFAULT (0xFFFFFFFF) FOR [T87]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF__tbl_inven__S88__1EA25E4D]  DEFAULT ((0)) FOR [S88]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF__tbl_inven__T88__1F968286]  DEFAULT (0xFFFFFFFF) FOR [T88]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF__tbl_inven__S89__208AA6BF]  DEFAULT ((0)) FOR [S89]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF__tbl_inven__T89__217ECAF8]  DEFAULT (0xFFFFFFFF) FOR [T89]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF__tbl_inven__S90__2272EF31]  DEFAULT ((0)) FOR [S90]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF__tbl_inven__T90__2367136A]  DEFAULT (0xFFFFFFFF) FOR [T90]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF__tbl_inven__S91__245B37A3]  DEFAULT ((0)) FOR [S91]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF__tbl_inven__T91__254F5BDC]  DEFAULT (0xFFFFFFFF) FOR [T91]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF__tbl_inven__S92__26438015]  DEFAULT ((0)) FOR [S92]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF__tbl_inven__T92__2737A44E]  DEFAULT (0xFFFFFFFF) FOR [T92]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF__tbl_inven__S93__282BC887]  DEFAULT ((0)) FOR [S93]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF__tbl_inven__T93__291FECC0]  DEFAULT (0xFFFFFFFF) FOR [T93]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF__tbl_inven__S94__2A1410F9]  DEFAULT ((0)) FOR [S94]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF__tbl_inven__T94__2B083532]  DEFAULT (0xFFFFFFFF) FOR [T94]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF__tbl_inven__S95__2BFC596B]  DEFAULT ((0)) FOR [S95]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF__tbl_inven__T95__2CF07DA4]  DEFAULT (0xFFFFFFFF) FOR [T95]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF__tbl_inven__S96__2DE4A1DD]  DEFAULT ((0)) FOR [S96]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF__tbl_inven__T96__2ED8C616]  DEFAULT (0xFFFFFFFF) FOR [T96]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF__tbl_inven__S97__2FCCEA4F]  DEFAULT ((0)) FOR [S97]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF__tbl_inven__T97__30C10E88]  DEFAULT (0xFFFFFFFF) FOR [T97]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF__tbl_inven__S98__31B532C1]  DEFAULT ((0)) FOR [S98]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF__tbl_inven__T98__32A956FA]  DEFAULT (0xFFFFFFFF) FOR [T98]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF__tbl_inven__S99__339D7B33]  DEFAULT ((0)) FOR [S99]
GO
ALTER TABLE [dbo].[tbl_inven] ADD  CONSTRAINT [DF__tbl_inven__T99__34919F6C]  DEFAULT (0xFFFFFFFF) FOR [T99]
GO
ALTER TABLE [dbo].[tbl_ItemCharge] ADD  CONSTRAINT [DF_tbl_ItemCharge_nItemCode_K]  DEFAULT (0xFFFFFFFF) FOR [nItemCode_K]
GO
ALTER TABLE [dbo].[tbl_ItemCharge] ADD  CONSTRAINT [DF_tbl_ItemCharge_nItemCode_D]  DEFAULT ((0)) FOR [nItemCode_D]
GO
ALTER TABLE [dbo].[tbl_ItemCharge] ADD  CONSTRAINT [DF_tbl_ItemCharge_nItemCode_U]  DEFAULT (0x0FFFFFFF) FOR [nItemCode_U]
GO
ALTER TABLE [dbo].[tbl_ItemCharge] ADD  CONSTRAINT [DF_tbl_ItemCharge_dtGiveDate]  DEFAULT (getdate()) FOR [dtGiveDate]
GO
ALTER TABLE [dbo].[tbl_ItemCharge] ADD  CONSTRAINT [DF_tbl_ItemCharge_dtTakeDate]  DEFAULT ((0)) FOR [dtTakeDate]
GO
ALTER TABLE [dbo].[tbl_ItemCharge] ADD  CONSTRAINT [DF_tbl_ItemCharge_DCK]  DEFAULT ((0)) FOR [DCK]
GO
ALTER TABLE [dbo].[tbl_ItemCharge] ADD  CONSTRAINT [DF_tbl_ItemCharge_Type]  DEFAULT ((0)) FOR [Type]
GO
ALTER TABLE [dbo].[tbl_ItemCharge] ADD  CONSTRAINT [DF__tbl_itemcharg__S__66B30AE5]  DEFAULT ((0)) FOR [S]
GO
ALTER TABLE [dbo].[tbl_ItemCharge] ADD  CONSTRAINT [DF__tbl_itemcharg__T__67A72F1E]  DEFAULT (0xFFFFFFFF) FOR [T]
GO
ALTER TABLE [dbo].[tbl_itemcombine_ex_result] ADD  CONSTRAINT [DF_tbl_itemcombine_ex_result_DCK]  DEFAULT ((0)) FOR [DCK]
GO
ALTER TABLE [dbo].[tbl_itemcombine_ex_result] ADD  CONSTRAINT [DF_tbl_itemcombine_ex_result_IsResult]  DEFAULT ((0)) FOR [IsResult]
GO
ALTER TABLE [dbo].[tbl_itemcombine_ex_result] ADD  CONSTRAINT [DF_tbl_itemcombine_ex_result_CheckKey]  DEFAULT (0xFFFFFFFF) FOR [CheckKey]
GO
ALTER TABLE [dbo].[tbl_itemcombine_ex_result] ADD  CONSTRAINT [DF_tbl_itemcombine_ex_result_DlgType]  DEFAULT (0xFF) FOR [DlgType]
GO
ALTER TABLE [dbo].[tbl_itemcombine_ex_result] ADD  CONSTRAINT [DF_tbl_itemcombine_ex_result_Dalant]  DEFAULT ((0)) FOR [Dalant]
GO
ALTER TABLE [dbo].[tbl_itemcombine_ex_result] ADD  CONSTRAINT [DF_tbl_itemcombine_ex_result_ItemListCount]  DEFAULT ((0)) FOR [ItemListCount]
GO
ALTER TABLE [dbo].[tbl_itemcombine_ex_result] ADD  CONSTRAINT [DF_tbl_itemcombine_ex_result_ItemSeletCount]  DEFAULT ((0)) FOR [ItemSelectCount]
GO
ALTER TABLE [dbo].[tbl_itemcombine_ex_result] ADD  CONSTRAINT [DF_tbl_itemcombine_ex_result_K0]  DEFAULT (0xFFFFFF) FOR [K0]
GO
ALTER TABLE [dbo].[tbl_itemcombine_ex_result] ADD  CONSTRAINT [DF_tbl_itemcombine_ex_result_D0]  DEFAULT ((0)) FOR [D0]
GO
ALTER TABLE [dbo].[tbl_itemcombine_ex_result] ADD  CONSTRAINT [DF_tbl_itemcombine_ex_result_U0]  DEFAULT (0xFFFFFF) FOR [U0]
GO
ALTER TABLE [dbo].[tbl_itemcombine_ex_result] ADD  CONSTRAINT [DF_tbl_itemcombine_ex_result_K1]  DEFAULT (0xFFFFFF) FOR [K1]
GO
ALTER TABLE [dbo].[tbl_itemcombine_ex_result] ADD  CONSTRAINT [DF_tbl_itemcombine_ex_result_D1]  DEFAULT ((0)) FOR [D1]
GO
ALTER TABLE [dbo].[tbl_itemcombine_ex_result] ADD  CONSTRAINT [DF_tbl_itemcombine_ex_result_U1]  DEFAULT (0xFFFFFF) FOR [U1]
GO
ALTER TABLE [dbo].[tbl_itemcombine_ex_result] ADD  CONSTRAINT [DF_tbl_itemcombine_ex_result_K2]  DEFAULT (0xFFFFFF) FOR [K2]
GO
ALTER TABLE [dbo].[tbl_itemcombine_ex_result] ADD  CONSTRAINT [DF_tbl_itemcombine_ex_result_D2]  DEFAULT ((0)) FOR [D2]
GO
ALTER TABLE [dbo].[tbl_itemcombine_ex_result] ADD  CONSTRAINT [DF_tbl_itemcombine_ex_result_U2]  DEFAULT (0xFFFFFF) FOR [U2]
GO
ALTER TABLE [dbo].[tbl_itemcombine_ex_result] ADD  CONSTRAINT [DF_tbl_itemcombine_ex_result_K3]  DEFAULT (0xFFFFFF) FOR [K3]
GO
ALTER TABLE [dbo].[tbl_itemcombine_ex_result] ADD  CONSTRAINT [DF_tbl_itemcombine_ex_result_D3]  DEFAULT ((0)) FOR [D3]
GO
ALTER TABLE [dbo].[tbl_itemcombine_ex_result] ADD  CONSTRAINT [DF_tbl_itemcombine_ex_result_U3]  DEFAULT (0xFFFFFF) FOR [U3]
GO
ALTER TABLE [dbo].[tbl_itemcombine_ex_result] ADD  CONSTRAINT [DF_tbl_itemcombine_ex_result_K4]  DEFAULT (0xFFFFFF) FOR [K4]
GO
ALTER TABLE [dbo].[tbl_itemcombine_ex_result] ADD  CONSTRAINT [DF_tbl_itemcombine_ex_result_D4]  DEFAULT ((0)) FOR [D4]
GO
ALTER TABLE [dbo].[tbl_itemcombine_ex_result] ADD  CONSTRAINT [DF_tbl_itemcombine_ex_result_U4]  DEFAULT (0xFFFFFF) FOR [U4]
GO
ALTER TABLE [dbo].[tbl_itemcombine_ex_result] ADD  CONSTRAINT [DF_tbl_itemcombine_ex_result_K5]  DEFAULT (0xFFFFFF) FOR [K5]
GO
ALTER TABLE [dbo].[tbl_itemcombine_ex_result] ADD  CONSTRAINT [DF_tbl_itemcombine_ex_result_D5]  DEFAULT ((0)) FOR [D5]
GO
ALTER TABLE [dbo].[tbl_itemcombine_ex_result] ADD  CONSTRAINT [DF_tbl_itemcombine_ex_result_U5]  DEFAULT (0xFFFFFF) FOR [U5]
GO
ALTER TABLE [dbo].[tbl_itemcombine_ex_result] ADD  CONSTRAINT [DF_tbl_itemcombine_ex_result_K6]  DEFAULT (0xFFFFFF) FOR [K6]
GO
ALTER TABLE [dbo].[tbl_itemcombine_ex_result] ADD  CONSTRAINT [DF_tbl_itemcombine_ex_result_D6]  DEFAULT ((0)) FOR [D6]
GO
ALTER TABLE [dbo].[tbl_itemcombine_ex_result] ADD  CONSTRAINT [DF_tbl_itemcombine_ex_result_U6]  DEFAULT (0xFFFFFF) FOR [U6]
GO
ALTER TABLE [dbo].[tbl_itemcombine_ex_result] ADD  CONSTRAINT [DF_tbl_itemcombine_ex_result_K7]  DEFAULT (0xFFFFFF) FOR [K7]
GO
ALTER TABLE [dbo].[tbl_itemcombine_ex_result] ADD  CONSTRAINT [DF_tbl_itemcombine_ex_result_D7]  DEFAULT ((0)) FOR [D7]
GO
ALTER TABLE [dbo].[tbl_itemcombine_ex_result] ADD  CONSTRAINT [DF_tbl_itemcombine_ex_result_U7]  DEFAULT (0xFFFFFF) FOR [U7]
GO
ALTER TABLE [dbo].[tbl_itemcombine_ex_result] ADD  CONSTRAINT [DF_tbl_itemcombine_ex_result_K8]  DEFAULT (0xFFFFFF) FOR [K8]
GO
ALTER TABLE [dbo].[tbl_itemcombine_ex_result] ADD  CONSTRAINT [DF_tbl_itemcombine_ex_result_D8]  DEFAULT ((0)) FOR [D8]
GO
ALTER TABLE [dbo].[tbl_itemcombine_ex_result] ADD  CONSTRAINT [DF_tbl_itemcombine_ex_result_U8]  DEFAULT (0xFFFFFF) FOR [U8]
GO
ALTER TABLE [dbo].[tbl_itemcombine_ex_result] ADD  CONSTRAINT [DF_tbl_itemcombine_ex_result_K9]  DEFAULT (0xFFFFFF) FOR [K9]
GO
ALTER TABLE [dbo].[tbl_itemcombine_ex_result] ADD  CONSTRAINT [DF_tbl_itemcombine_ex_result_D9]  DEFAULT ((0)) FOR [D9]
GO
ALTER TABLE [dbo].[tbl_itemcombine_ex_result] ADD  CONSTRAINT [DF_tbl_itemcombine_ex_result_U9]  DEFAULT (0xFFFFFF) FOR [U9]
GO
ALTER TABLE [dbo].[tbl_itemcombine_ex_result] ADD  CONSTRAINT [DF_tbl_itemcombine_ex_result_K10]  DEFAULT (0xFFFFFF) FOR [K10]
GO
ALTER TABLE [dbo].[tbl_itemcombine_ex_result] ADD  CONSTRAINT [DF_tbl_itemcombine_ex_result_D10]  DEFAULT ((0)) FOR [D10]
GO
ALTER TABLE [dbo].[tbl_itemcombine_ex_result] ADD  CONSTRAINT [DF_tbl_itemcombine_ex_result_U10]  DEFAULT (0xFFFFFF) FOR [U10]
GO
ALTER TABLE [dbo].[tbl_itemcombine_ex_result] ADD  CONSTRAINT [DF_tbl_itemcombine_ex_result_K11]  DEFAULT (0xFFFFFF) FOR [K11]
GO
ALTER TABLE [dbo].[tbl_itemcombine_ex_result] ADD  CONSTRAINT [DF_tbl_itemcombine_ex_result_D11]  DEFAULT ((0)) FOR [D11]
GO
ALTER TABLE [dbo].[tbl_itemcombine_ex_result] ADD  CONSTRAINT [DF_tbl_itemcombine_ex_result_U11]  DEFAULT (0xFFFFFF) FOR [U11]
GO
ALTER TABLE [dbo].[tbl_itemcombine_ex_result] ADD  CONSTRAINT [DF_tbl_itemcombine_ex_result_K12]  DEFAULT (0xFFFFFF) FOR [K12]
GO
ALTER TABLE [dbo].[tbl_itemcombine_ex_result] ADD  CONSTRAINT [DF_tbl_itemcombine_ex_result_D12]  DEFAULT ((0)) FOR [D12]
GO
ALTER TABLE [dbo].[tbl_itemcombine_ex_result] ADD  CONSTRAINT [DF_tbl_itemcombine_ex_result_U12]  DEFAULT (0xFFFFFF) FOR [U12]
GO
ALTER TABLE [dbo].[tbl_itemcombine_ex_result] ADD  CONSTRAINT [DF_tbl_itemcombine_ex_result_K13]  DEFAULT (0xFFFFFF) FOR [K13]
GO
ALTER TABLE [dbo].[tbl_itemcombine_ex_result] ADD  CONSTRAINT [DF_tbl_itemcombine_ex_result_D13]  DEFAULT ((0)) FOR [D13]
GO
ALTER TABLE [dbo].[tbl_itemcombine_ex_result] ADD  CONSTRAINT [DF_tbl_itemcombine_ex_result_U13]  DEFAULT (0xFFFFFF) FOR [U13]
GO
ALTER TABLE [dbo].[tbl_itemcombine_ex_result] ADD  CONSTRAINT [DF_tbl_itemcombine_ex_result_K14]  DEFAULT (0xFFFFFF) FOR [K14]
GO
ALTER TABLE [dbo].[tbl_itemcombine_ex_result] ADD  CONSTRAINT [DF_tbl_itemcombine_ex_result_D14]  DEFAULT ((0)) FOR [D14]
GO
ALTER TABLE [dbo].[tbl_itemcombine_ex_result] ADD  CONSTRAINT [DF_tbl_itemcombine_ex_result_U14]  DEFAULT (0xFFFFFF) FOR [U14]
GO
ALTER TABLE [dbo].[tbl_itemcombine_ex_result] ADD  CONSTRAINT [DF_tbl_itemcombine_ex_result_K15]  DEFAULT (0xFFFFFF) FOR [K15]
GO
ALTER TABLE [dbo].[tbl_itemcombine_ex_result] ADD  CONSTRAINT [DF_tbl_itemcombine_ex_result_D15]  DEFAULT ((0)) FOR [D15]
GO
ALTER TABLE [dbo].[tbl_itemcombine_ex_result] ADD  CONSTRAINT [DF_tbl_itemcombine_ex_result_U15]  DEFAULT (0xFFFFFF) FOR [U15]
GO
ALTER TABLE [dbo].[tbl_itemcombine_ex_result] ADD  CONSTRAINT [DF_tbl_itemcombine_ex_result_K16]  DEFAULT (0xFFFFFF) FOR [K16]
GO
ALTER TABLE [dbo].[tbl_itemcombine_ex_result] ADD  CONSTRAINT [DF_tbl_itemcombine_ex_result_D16]  DEFAULT ((0)) FOR [D16]
GO
ALTER TABLE [dbo].[tbl_itemcombine_ex_result] ADD  CONSTRAINT [DF_tbl_itemcombine_ex_result_U16]  DEFAULT (0xFFFFFF) FOR [U16]
GO
ALTER TABLE [dbo].[tbl_itemcombine_ex_result] ADD  CONSTRAINT [DF_tbl_itemcombine_ex_result_K17]  DEFAULT (0xFFFFFF) FOR [K17]
GO
ALTER TABLE [dbo].[tbl_itemcombine_ex_result] ADD  CONSTRAINT [DF_tbl_itemcombine_ex_result_D17]  DEFAULT ((0)) FOR [D17]
GO
ALTER TABLE [dbo].[tbl_itemcombine_ex_result] ADD  CONSTRAINT [DF_tbl_itemcombine_ex_result_U17]  DEFAULT (0xFFFFFF) FOR [U17]
GO
ALTER TABLE [dbo].[tbl_itemcombine_ex_result] ADD  CONSTRAINT [DF_tbl_itemcombine_ex_result_K18]  DEFAULT (0xFFFFFF) FOR [K18]
GO
ALTER TABLE [dbo].[tbl_itemcombine_ex_result] ADD  CONSTRAINT [DF_tbl_itemcombine_ex_result_D18]  DEFAULT ((0)) FOR [D18]
GO
ALTER TABLE [dbo].[tbl_itemcombine_ex_result] ADD  CONSTRAINT [DF_tbl_itemcombine_ex_result_U18]  DEFAULT (0xFFFFFF) FOR [U18]
GO
ALTER TABLE [dbo].[tbl_itemcombine_ex_result] ADD  CONSTRAINT [DF_tbl_itemcombine_ex_result_K19]  DEFAULT (0xFFFFFF) FOR [K19]
GO
ALTER TABLE [dbo].[tbl_itemcombine_ex_result] ADD  CONSTRAINT [DF_tbl_itemcombine_ex_result_D19]  DEFAULT ((0)) FOR [D19]
GO
ALTER TABLE [dbo].[tbl_itemcombine_ex_result] ADD  CONSTRAINT [DF_tbl_itemcombine_ex_result_U19]  DEFAULT (0xFFFFFF) FOR [U19]
GO
ALTER TABLE [dbo].[tbl_itemcombine_ex_result] ADD  CONSTRAINT [DF_tbl_itemcombine_ex_result_K20]  DEFAULT (0xFFFFFF) FOR [K20]
GO
ALTER TABLE [dbo].[tbl_itemcombine_ex_result] ADD  CONSTRAINT [DF_tbl_itemcombine_ex_result_D20]  DEFAULT ((0)) FOR [D20]
GO
ALTER TABLE [dbo].[tbl_itemcombine_ex_result] ADD  CONSTRAINT [DF_tbl_itemcombine_ex_result_U20]  DEFAULT (0xFFFFFF) FOR [U20]
GO
ALTER TABLE [dbo].[tbl_itemcombine_ex_result] ADD  CONSTRAINT [DF_tbl_itemcombine_ex_result_K21]  DEFAULT (0xFFFFFF) FOR [K21]
GO
ALTER TABLE [dbo].[tbl_itemcombine_ex_result] ADD  CONSTRAINT [DF_tbl_itemcombine_ex_result_D21]  DEFAULT ((0)) FOR [D21]
GO
ALTER TABLE [dbo].[tbl_itemcombine_ex_result] ADD  CONSTRAINT [DF_tbl_itemcombine_ex_result_U21]  DEFAULT (0xFFFFFF) FOR [U21]
GO
ALTER TABLE [dbo].[tbl_itemcombine_ex_result] ADD  CONSTRAINT [DF_tbl_itemcombine_ex_result_K22]  DEFAULT (0xFFFFFF) FOR [K22]
GO
ALTER TABLE [dbo].[tbl_itemcombine_ex_result] ADD  CONSTRAINT [DF_tbl_itemcombine_ex_result_D22]  DEFAULT ((0)) FOR [D22]
GO
ALTER TABLE [dbo].[tbl_itemcombine_ex_result] ADD  CONSTRAINT [DF_tbl_itemcombine_ex_result_U22]  DEFAULT (0xFFFFFF) FOR [U22]
GO
ALTER TABLE [dbo].[tbl_itemcombine_ex_result] ADD  CONSTRAINT [DF_tbl_itemcombine_ex_result_K23]  DEFAULT (0xFFFFFF) FOR [K23]
GO
ALTER TABLE [dbo].[tbl_itemcombine_ex_result] ADD  CONSTRAINT [DF_tbl_itemcombine_ex_result_D23]  DEFAULT ((0)) FOR [D23]
GO
ALTER TABLE [dbo].[tbl_itemcombine_ex_result] ADD  CONSTRAINT [DF_tbl_itemcombine_ex_result_U23]  DEFAULT (0xFFFFFF) FOR [U23]
GO
ALTER TABLE [dbo].[tbl_itemcombine_ex_result] ADD  CONSTRAINT [DF_tbl_itemcombine_ex_result_resulteffecttype]  DEFAULT ((0)) FOR [resulteffecttype]
GO
ALTER TABLE [dbo].[tbl_itemcombine_ex_result] ADD  CONSTRAINT [DF_tbl_itemcombine_ex_resulteffectmsgcode]  DEFAULT ((0)) FOR [resulteffectmsgcode]
GO
ALTER TABLE [dbo].[tbl_log_lv] ADD  CONSTRAINT [DF_tbl_log_lv_AvatorSerial]  DEFAULT ((0)) FOR [AvatorSerial]
GO
ALTER TABLE [dbo].[tbl_log_lv] ADD  CONSTRAINT [DF_tbl_log_lv_Lv]  DEFAULT ((0)) FOR [Lv]
GO
ALTER TABLE [dbo].[tbl_log_lv] ADD  CONSTRAINT [DF_tbl_log_lv_Date]  DEFAULT (getdate()) FOR [dtDate]
GO
ALTER TABLE [dbo].[tbl_log_lv] ADD  CONSTRAINT [DF_tbl_log_lv_TotalMin]  DEFAULT ((0)) FOR [TotalMin]
GO
ALTER TABLE [dbo].[tbl_log_usernum] ADD  CONSTRAINT [DF_tbl_UserNum_dtDate]  DEFAULT (getdate()) FOR [dtDate]
GO
ALTER TABLE [dbo].[tbl_log_usernum] ADD  CONSTRAINT [DF_tbl_UserNum_nAverageUser]  DEFAULT ((0)) FOR [nAverageUser]
GO
ALTER TABLE [dbo].[tbl_log_usernum] ADD  CONSTRAINT [DF_tbl_UserNum_nMaxUser]  DEFAULT ((0)) FOR [nMaxUser]
GO
ALTER TABLE [dbo].[tbl_Macro] ADD  CONSTRAINT [DF__tbl_Macro__hp__32024716]  DEFAULT (0xFFFFFFFF) FOR [hp]
GO
ALTER TABLE [dbo].[tbl_Macro] ADD  CONSTRAINT [DF__tbl_Macro__fp__32F66B4F]  DEFAULT (0xFFFFFFFF) FOR [fp]
GO
ALTER TABLE [dbo].[tbl_Macro] ADD  CONSTRAINT [DF__tbl_Macro__sp__33EA8F88]  DEFAULT (0xFFFFFFFF) FOR [sp]
GO
ALTER TABLE [dbo].[tbl_Macro] ADD  CONSTRAINT [DF__tbl_Macro__actio__34DEB3C1]  DEFAULT (0xFFFFFFFF) FOR [action0]
GO
ALTER TABLE [dbo].[tbl_Macro] ADD  CONSTRAINT [DF__tbl_Macro__actio__35D2D7FA]  DEFAULT (0xFFFFFFFF) FOR [action1]
GO
ALTER TABLE [dbo].[tbl_Macro] ADD  CONSTRAINT [DF__tbl_Macro__actio__36C6FC33]  DEFAULT (0xFFFFFFFF) FOR [action2]
GO
ALTER TABLE [dbo].[tbl_Macro] ADD  CONSTRAINT [DF__tbl_Macro__actio__37BB206C]  DEFAULT (0xFFFFFFFF) FOR [action3]
GO
ALTER TABLE [dbo].[tbl_Macro] ADD  CONSTRAINT [DF__tbl_Macro__actio__38AF44A5]  DEFAULT (0xFFFFFFFF) FOR [action4]
GO
ALTER TABLE [dbo].[tbl_Macro] ADD  CONSTRAINT [DF__tbl_Macro__actio__39A368DE]  DEFAULT (0xFFFFFFFF) FOR [action5]
GO
ALTER TABLE [dbo].[tbl_Macro] ADD  CONSTRAINT [DF__tbl_Macro__actio__3A978D17]  DEFAULT (0xFFFFFFFF) FOR [action6]
GO
ALTER TABLE [dbo].[tbl_Macro] ADD  CONSTRAINT [DF__tbl_Macro__actio__3B8BB150]  DEFAULT (0xFFFFFFFF) FOR [action7]
GO
ALTER TABLE [dbo].[tbl_Macro] ADD  CONSTRAINT [DF__tbl_Macro__actio__3C7FD589]  DEFAULT (0xFFFFFFFF) FOR [action8]
GO
ALTER TABLE [dbo].[tbl_Macro] ADD  CONSTRAINT [DF__tbl_Macro__actio__3D73F9C2]  DEFAULT (0xFFFFFFFF) FOR [action9]
GO
ALTER TABLE [dbo].[tbl_Macro] ADD  CONSTRAINT [DF__tbl_Macro__hpval__3E681DFB]  DEFAULT ((0)) FOR [hpvalue]
GO
ALTER TABLE [dbo].[tbl_Macro] ADD  CONSTRAINT [DF__tbl_Macro__fpval__3F5C4234]  DEFAULT ((0)) FOR [fpvalue]
GO
ALTER TABLE [dbo].[tbl_Macro] ADD  CONSTRAINT [DF__tbl_Macro__spval__4050666D]  DEFAULT ((0)) FOR [spvalue]
GO
ALTER TABLE [dbo].[tbl_npc_quest_history] ADD  CONSTRAINT [DF_tbl_npc_quest_Code1]  DEFAULT ('*') FOR [Code1]
GO
ALTER TABLE [dbo].[tbl_npc_quest_history] ADD  CONSTRAINT [DF__tbl_npc_q__Level__344A823B]  DEFAULT (0xFF) FOR [Level1]
GO
ALTER TABLE [dbo].[tbl_npc_quest_history] ADD  CONSTRAINT [DF_tbl_npc_quest_Code2]  DEFAULT ('*') FOR [Code2]
GO
ALTER TABLE [dbo].[tbl_npc_quest_history] ADD  CONSTRAINT [DF__tbl_npc_q__Level__3632CAAD]  DEFAULT (0xFF) FOR [Level2]
GO
ALTER TABLE [dbo].[tbl_npc_quest_history] ADD  CONSTRAINT [DF_tbl_npc_quest_Code3]  DEFAULT ('*') FOR [Code3]
GO
ALTER TABLE [dbo].[tbl_npc_quest_history] ADD  CONSTRAINT [DF__tbl_npc_q__Level__381B131F]  DEFAULT (0xFF) FOR [Level3]
GO
ALTER TABLE [dbo].[tbl_npc_quest_history] ADD  CONSTRAINT [DF_tbl_npc_quest_Code4]  DEFAULT ('*') FOR [Code4]
GO
ALTER TABLE [dbo].[tbl_npc_quest_history] ADD  CONSTRAINT [DF__tbl_npc_q__Level__3A035B91]  DEFAULT (0xFF) FOR [Level4]
GO
ALTER TABLE [dbo].[tbl_npc_quest_history] ADD  CONSTRAINT [DF_tbl_npc_quest_Code5]  DEFAULT ('*') FOR [Code5]
GO
ALTER TABLE [dbo].[tbl_npc_quest_history] ADD  CONSTRAINT [DF__tbl_npc_q__Level__3BEBA403]  DEFAULT (0xFF) FOR [Level5]
GO
ALTER TABLE [dbo].[tbl_npc_quest_history] ADD  CONSTRAINT [DF_tbl_npc_quest_Code6]  DEFAULT ('*') FOR [Code6]
GO
ALTER TABLE [dbo].[tbl_npc_quest_history] ADD  CONSTRAINT [DF__tbl_npc_q__Level__3DD3EC75]  DEFAULT (0xFF) FOR [Level6]
GO
ALTER TABLE [dbo].[tbl_npc_quest_history] ADD  CONSTRAINT [DF_tbl_npc_quest_Code7]  DEFAULT ('*') FOR [Code7]
GO
ALTER TABLE [dbo].[tbl_npc_quest_history] ADD  CONSTRAINT [DF__tbl_npc_q__Level__3FBC34E7]  DEFAULT (0xFF) FOR [Level7]
GO
ALTER TABLE [dbo].[tbl_npc_quest_history] ADD  CONSTRAINT [DF_tbl_npc_quest_Code8]  DEFAULT ('*') FOR [Code8]
GO
ALTER TABLE [dbo].[tbl_npc_quest_history] ADD  CONSTRAINT [DF__tbl_npc_q__Level__41A47D59]  DEFAULT (0xFF) FOR [Level8]
GO
ALTER TABLE [dbo].[tbl_npc_quest_history] ADD  CONSTRAINT [DF_tbl_npc_quest_Code9]  DEFAULT ('*') FOR [Code9]
GO
ALTER TABLE [dbo].[tbl_npc_quest_history] ADD  CONSTRAINT [DF__tbl_npc_q__Level__438CC5CB]  DEFAULT (0xFF) FOR [Level9]
GO
ALTER TABLE [dbo].[tbl_npc_quest_history] ADD  CONSTRAINT [DF_tbl_npc_quest_Code10]  DEFAULT ('*') FOR [Code10]
GO
ALTER TABLE [dbo].[tbl_npc_quest_history] ADD  CONSTRAINT [DF__tbl_npc_q__Level__45750E3D]  DEFAULT (0xFF) FOR [Level10]
GO
ALTER TABLE [dbo].[tbl_npc_quest_history] ADD  CONSTRAINT [DF_tbl_npc_quest_Code11]  DEFAULT ('*') FOR [Code11]
GO
ALTER TABLE [dbo].[tbl_npc_quest_history] ADD  CONSTRAINT [DF__tbl_npc_q__Level__475D56AF]  DEFAULT (0xFF) FOR [Level11]
GO
ALTER TABLE [dbo].[tbl_npc_quest_history] ADD  CONSTRAINT [DF_tbl_npc_quest_Code12]  DEFAULT ('*') FOR [Code12]
GO
ALTER TABLE [dbo].[tbl_npc_quest_history] ADD  CONSTRAINT [DF__tbl_npc_q__Level__49459F21]  DEFAULT (0xFF) FOR [Level12]
GO
ALTER TABLE [dbo].[tbl_npc_quest_history] ADD  CONSTRAINT [DF_tbl_npc_quest_Code13]  DEFAULT ('*') FOR [Code13]
GO
ALTER TABLE [dbo].[tbl_npc_quest_history] ADD  CONSTRAINT [DF__tbl_npc_q__Level__4B2DE793]  DEFAULT (0xFF) FOR [Level13]
GO
ALTER TABLE [dbo].[tbl_npc_quest_history] ADD  CONSTRAINT [DF_tbl_npc_quest_Code14]  DEFAULT ('*') FOR [Code14]
GO
ALTER TABLE [dbo].[tbl_npc_quest_history] ADD  CONSTRAINT [DF__tbl_npc_q__Level__4D163005]  DEFAULT (0xFF) FOR [Level14]
GO
ALTER TABLE [dbo].[tbl_npc_quest_history] ADD  CONSTRAINT [DF_tbl_npc_quest_Code15]  DEFAULT ('*') FOR [Code15]
GO
ALTER TABLE [dbo].[tbl_npc_quest_history] ADD  CONSTRAINT [DF__tbl_npc_q__Level__4EFE7877]  DEFAULT (0xFF) FOR [Level15]
GO
ALTER TABLE [dbo].[tbl_npc_quest_history] ADD  CONSTRAINT [DF_tbl_npc_quest_Code16]  DEFAULT ('*') FOR [Code16]
GO
ALTER TABLE [dbo].[tbl_npc_quest_history] ADD  CONSTRAINT [DF__tbl_npc_q__Level__50E6C0E9]  DEFAULT (0xFF) FOR [Level16]
GO
ALTER TABLE [dbo].[tbl_npc_quest_history] ADD  CONSTRAINT [DF_tbl_npc_quest_Code17]  DEFAULT ('*') FOR [Code17]
GO
ALTER TABLE [dbo].[tbl_npc_quest_history] ADD  CONSTRAINT [DF__tbl_npc_q__Level__52CF095B]  DEFAULT (0xFF) FOR [Level17]
GO
ALTER TABLE [dbo].[tbl_npc_quest_history] ADD  CONSTRAINT [DF_tbl_npc_quest_Code18]  DEFAULT ('*') FOR [Code18]
GO
ALTER TABLE [dbo].[tbl_npc_quest_history] ADD  CONSTRAINT [DF__tbl_npc_q__Level__54B751CD]  DEFAULT (0xFF) FOR [Level18]
GO
ALTER TABLE [dbo].[tbl_npc_quest_history] ADD  CONSTRAINT [DF_tbl_npc_quest_Code19]  DEFAULT ('*') FOR [Code19]
GO
ALTER TABLE [dbo].[tbl_npc_quest_history] ADD  CONSTRAINT [DF__tbl_npc_q__Level__569F9A3F]  DEFAULT (0xFF) FOR [Level19]
GO
ALTER TABLE [dbo].[tbl_npc_quest_history] ADD  CONSTRAINT [DF_tbl_npc_quest_Code20]  DEFAULT ('*') FOR [Code20]
GO
ALTER TABLE [dbo].[tbl_npc_quest_history] ADD  CONSTRAINT [DF__tbl_npc_q__Level__5887E2B1]  DEFAULT (0xFF) FOR [Level20]
GO
ALTER TABLE [dbo].[tbl_npc_quest_history] ADD  CONSTRAINT [DF_tbl_npc_quest_Time1]  DEFAULT ((0)) FOR [Time1]
GO
ALTER TABLE [dbo].[tbl_npc_quest_history] ADD  CONSTRAINT [DF_tbl_npc_quest_Time2]  DEFAULT ((0)) FOR [Time2]
GO
ALTER TABLE [dbo].[tbl_npc_quest_history] ADD  CONSTRAINT [DF_tbl_npc_quest_Time3]  DEFAULT ((0)) FOR [Time3]
GO
ALTER TABLE [dbo].[tbl_npc_quest_history] ADD  CONSTRAINT [DF_tbl_npc_quest_Time4]  DEFAULT ((0)) FOR [Time4]
GO
ALTER TABLE [dbo].[tbl_npc_quest_history] ADD  CONSTRAINT [DF_tbl_npc_quest_Time5]  DEFAULT ((0)) FOR [Time5]
GO
ALTER TABLE [dbo].[tbl_npc_quest_history] ADD  CONSTRAINT [DF_tbl_npc_quest_Time6]  DEFAULT ((0)) FOR [Time6]
GO
ALTER TABLE [dbo].[tbl_npc_quest_history] ADD  CONSTRAINT [DF_tbl_npc_quest_Time7]  DEFAULT ((0)) FOR [Time7]
GO
ALTER TABLE [dbo].[tbl_npc_quest_history] ADD  CONSTRAINT [DF_tbl_npc_quest_Time8]  DEFAULT ((0)) FOR [Time8]
GO
ALTER TABLE [dbo].[tbl_npc_quest_history] ADD  CONSTRAINT [DF_tbl_npc_quest_Time9]  DEFAULT ((0)) FOR [Time9]
GO
ALTER TABLE [dbo].[tbl_npc_quest_history] ADD  CONSTRAINT [DF_tbl_npc_quest_Time10]  DEFAULT ((0)) FOR [Time10]
GO
ALTER TABLE [dbo].[tbl_npc_quest_history] ADD  CONSTRAINT [DF_tbl_npc_quest_Time11]  DEFAULT ((0)) FOR [Time11]
GO
ALTER TABLE [dbo].[tbl_npc_quest_history] ADD  CONSTRAINT [DF_tbl_npc_quest_Time12]  DEFAULT ((0)) FOR [Time12]
GO
ALTER TABLE [dbo].[tbl_npc_quest_history] ADD  CONSTRAINT [DF_tbl_npc_quest_Time13]  DEFAULT ((0)) FOR [Time13]
GO
ALTER TABLE [dbo].[tbl_npc_quest_history] ADD  CONSTRAINT [DF_tbl_npc_quest_Time14]  DEFAULT ((0)) FOR [Time14]
GO
ALTER TABLE [dbo].[tbl_npc_quest_history] ADD  CONSTRAINT [DF_tbl_npc_quest_Time15]  DEFAULT ((0)) FOR [Time15]
GO
ALTER TABLE [dbo].[tbl_npc_quest_history] ADD  CONSTRAINT [DF_tbl_npc_quest_Time16]  DEFAULT ((0)) FOR [Time16]
GO
ALTER TABLE [dbo].[tbl_npc_quest_history] ADD  CONSTRAINT [DF_tbl_npc_quest_Time17]  DEFAULT ((0)) FOR [Time17]
GO
ALTER TABLE [dbo].[tbl_npc_quest_history] ADD  CONSTRAINT [DF_tbl_npc_quest_Time18]  DEFAULT ((0)) FOR [Time18]
GO
ALTER TABLE [dbo].[tbl_npc_quest_history] ADD  CONSTRAINT [DF_tbl_npc_quest_Time19]  DEFAULT ((0)) FOR [Time19]
GO
ALTER TABLE [dbo].[tbl_npc_quest_history] ADD  CONSTRAINT [DF_tbl_npc_quest_Time20]  DEFAULT ((0)) FOR [Time20]
GO
ALTER TABLE [dbo].[tbl_npc_quest_history] ADD  CONSTRAINT [DF_tbl_npc_quest_Code21]  DEFAULT ('*') FOR [Code21]
GO
ALTER TABLE [dbo].[tbl_npc_quest_history] ADD  CONSTRAINT [DF_tbl_npc_quest_Level21]  DEFAULT (0xFF) FOR [Level21]
GO
ALTER TABLE [dbo].[tbl_npc_quest_history] ADD  CONSTRAINT [DF_tbl_npc_quest_Time21]  DEFAULT ((0)) FOR [Time21]
GO
ALTER TABLE [dbo].[tbl_npc_quest_history] ADD  CONSTRAINT [DF_tbl_npc_quest_Code22]  DEFAULT ('*') FOR [Code22]
GO
ALTER TABLE [dbo].[tbl_npc_quest_history] ADD  CONSTRAINT [DF_tbl_npc_quest_Level22]  DEFAULT (0xFF) FOR [Level22]
GO
ALTER TABLE [dbo].[tbl_npc_quest_history] ADD  CONSTRAINT [DF_tbl_npc_quest_Time22]  DEFAULT ((0)) FOR [Time22]
GO
ALTER TABLE [dbo].[tbl_npc_quest_history] ADD  CONSTRAINT [DF_tbl_npc_quest_Code23]  DEFAULT ('*') FOR [Code23]
GO
ALTER TABLE [dbo].[tbl_npc_quest_history] ADD  CONSTRAINT [DF_tbl_npc_quest_Level23]  DEFAULT (0xFF) FOR [Level23]
GO
ALTER TABLE [dbo].[tbl_npc_quest_history] ADD  CONSTRAINT [DF_tbl_npc_quest_Time23]  DEFAULT ((0)) FOR [Time23]
GO
ALTER TABLE [dbo].[tbl_npc_quest_history] ADD  CONSTRAINT [DF_tbl_npc_quest_Code24]  DEFAULT ('*') FOR [Code24]
GO
ALTER TABLE [dbo].[tbl_npc_quest_history] ADD  CONSTRAINT [DF_tbl_npc_quest_Level24]  DEFAULT (0xFF) FOR [Level24]
GO
ALTER TABLE [dbo].[tbl_npc_quest_history] ADD  CONSTRAINT [DF_tbl_npc_quest_Time24]  DEFAULT ((0)) FOR [Time24]
GO
ALTER TABLE [dbo].[tbl_npc_quest_history] ADD  CONSTRAINT [DF_tbl_npc_quest_Code25]  DEFAULT ('*') FOR [Code25]
GO
ALTER TABLE [dbo].[tbl_npc_quest_history] ADD  CONSTRAINT [DF_tbl_npc_quest_Level25]  DEFAULT (0xFF) FOR [Level25]
GO
ALTER TABLE [dbo].[tbl_npc_quest_history] ADD  CONSTRAINT [DF_tbl_npc_quest_Time25]  DEFAULT ((0)) FOR [Time25]
GO
ALTER TABLE [dbo].[tbl_npc_quest_history] ADD  CONSTRAINT [DF_tbl_npc_quest_Code26]  DEFAULT ('*') FOR [Code26]
GO
ALTER TABLE [dbo].[tbl_npc_quest_history] ADD  CONSTRAINT [DF_tbl_npc_quest_Level26]  DEFAULT (0xFF) FOR [Level26]
GO
ALTER TABLE [dbo].[tbl_npc_quest_history] ADD  CONSTRAINT [DF_tbl_npc_quest_Time26]  DEFAULT ((0)) FOR [Time26]
GO
ALTER TABLE [dbo].[tbl_npc_quest_history] ADD  CONSTRAINT [DF_tbl_npc_quest_Code27]  DEFAULT ('*') FOR [Code27]
GO
ALTER TABLE [dbo].[tbl_npc_quest_history] ADD  CONSTRAINT [DF_tbl_npc_quest_Level27]  DEFAULT (0xFF) FOR [Level27]
GO
ALTER TABLE [dbo].[tbl_npc_quest_history] ADD  CONSTRAINT [DF_tbl_npc_quest_Time27]  DEFAULT ((0)) FOR [Time27]
GO
ALTER TABLE [dbo].[tbl_npc_quest_history] ADD  CONSTRAINT [DF_tbl_npc_quest_Code28]  DEFAULT ('*') FOR [Code28]
GO
ALTER TABLE [dbo].[tbl_npc_quest_history] ADD  CONSTRAINT [DF_tbl_npc_quest_Level28]  DEFAULT (0xFF) FOR [Level28]
GO
ALTER TABLE [dbo].[tbl_npc_quest_history] ADD  CONSTRAINT [DF_tbl_npc_quest_Time28]  DEFAULT ((0)) FOR [Time28]
GO
ALTER TABLE [dbo].[tbl_npc_quest_history] ADD  CONSTRAINT [DF_tbl_npc_quest_Code29]  DEFAULT ('*') FOR [Code29]
GO
ALTER TABLE [dbo].[tbl_npc_quest_history] ADD  CONSTRAINT [DF_tbl_npc_quest_Level29]  DEFAULT (0xFF) FOR [Level29]
GO
ALTER TABLE [dbo].[tbl_npc_quest_history] ADD  CONSTRAINT [DF_tbl_npc_quest_Time29]  DEFAULT ((0)) FOR [Time29]
GO
ALTER TABLE [dbo].[tbl_npc_quest_history] ADD  CONSTRAINT [DF_tbl_npc_quest_Code30]  DEFAULT ('*') FOR [Code30]
GO
ALTER TABLE [dbo].[tbl_npc_quest_history] ADD  CONSTRAINT [DF_tbl_npc_quest_Level30]  DEFAULT (0xFF) FOR [Level30]
GO
ALTER TABLE [dbo].[tbl_npc_quest_history] ADD  CONSTRAINT [DF_tbl_npc_quest_Time30]  DEFAULT ((0)) FOR [Time30]
GO
ALTER TABLE [dbo].[tbl_npc_quest_history] ADD  CONSTRAINT [DF_tbl_npc_quest_Code31]  DEFAULT ('*') FOR [Code31]
GO
ALTER TABLE [dbo].[tbl_npc_quest_history] ADD  CONSTRAINT [DF_tbl_npc_quest_Level31]  DEFAULT (0xFF) FOR [Level31]
GO
ALTER TABLE [dbo].[tbl_npc_quest_history] ADD  CONSTRAINT [DF_tbl_npc_quest_Time31]  DEFAULT ((0)) FOR [Time31]
GO
ALTER TABLE [dbo].[tbl_npc_quest_history] ADD  CONSTRAINT [DF_tbl_npc_quest_Code32]  DEFAULT ('*') FOR [Code32]
GO
ALTER TABLE [dbo].[tbl_npc_quest_history] ADD  CONSTRAINT [DF_tbl_npc_quest_Level32]  DEFAULT (0xFF) FOR [Level32]
GO
ALTER TABLE [dbo].[tbl_npc_quest_history] ADD  CONSTRAINT [DF_tbl_npc_quest_Time32]  DEFAULT ((0)) FOR [Time32]
GO
ALTER TABLE [dbo].[tbl_npc_quest_history] ADD  CONSTRAINT [DF_tbl_npc_quest_Code33]  DEFAULT ('*') FOR [Code33]
GO
ALTER TABLE [dbo].[tbl_npc_quest_history] ADD  CONSTRAINT [DF_tbl_npc_quest_Level33]  DEFAULT (0xFF) FOR [Level33]
GO
ALTER TABLE [dbo].[tbl_npc_quest_history] ADD  CONSTRAINT [DF_tbl_npc_quest_Time33]  DEFAULT ((0)) FOR [Time33]
GO
ALTER TABLE [dbo].[tbl_npc_quest_history] ADD  CONSTRAINT [DF_tbl_npc_quest_Code34]  DEFAULT ('*') FOR [Code34]
GO
ALTER TABLE [dbo].[tbl_npc_quest_history] ADD  CONSTRAINT [DF_tbl_npc_quest_Level34]  DEFAULT (0xFF) FOR [Level34]
GO
ALTER TABLE [dbo].[tbl_npc_quest_history] ADD  CONSTRAINT [DF_tbl_npc_quest_Time34]  DEFAULT ((0)) FOR [Time34]
GO
ALTER TABLE [dbo].[tbl_npc_quest_history] ADD  CONSTRAINT [DF_tbl_npc_quest_Code35]  DEFAULT ('*') FOR [Code35]
GO
ALTER TABLE [dbo].[tbl_npc_quest_history] ADD  CONSTRAINT [DF_tbl_npc_quest_Level35]  DEFAULT (0xFF) FOR [Level35]
GO
ALTER TABLE [dbo].[tbl_npc_quest_history] ADD  CONSTRAINT [DF_tbl_npc_quest_Time35]  DEFAULT ((0)) FOR [Time35]
GO
ALTER TABLE [dbo].[tbl_npc_quest_history] ADD  CONSTRAINT [DF_tbl_npc_quest_Code36]  DEFAULT ('*') FOR [Code36]
GO
ALTER TABLE [dbo].[tbl_npc_quest_history] ADD  CONSTRAINT [DF_tbl_npc_quest_Level36]  DEFAULT (0xFF) FOR [Level36]
GO
ALTER TABLE [dbo].[tbl_npc_quest_history] ADD  CONSTRAINT [DF_tbl_npc_quest_Time36]  DEFAULT ((0)) FOR [Time36]
GO
ALTER TABLE [dbo].[tbl_npc_quest_history] ADD  CONSTRAINT [DF_tbl_npc_quest_Code37]  DEFAULT ('*') FOR [Code37]
GO
ALTER TABLE [dbo].[tbl_npc_quest_history] ADD  CONSTRAINT [DF_tbl_npc_quest_Level37]  DEFAULT (0xFF) FOR [Level37]
GO
ALTER TABLE [dbo].[tbl_npc_quest_history] ADD  CONSTRAINT [DF_tbl_npc_quest_Time37]  DEFAULT ((0)) FOR [Time37]
GO
ALTER TABLE [dbo].[tbl_npc_quest_history] ADD  CONSTRAINT [DF_tbl_npc_quest_Code38]  DEFAULT ('*') FOR [Code38]
GO
ALTER TABLE [dbo].[tbl_npc_quest_history] ADD  CONSTRAINT [DF_tbl_npc_quest_Level38]  DEFAULT (0xFF) FOR [Level38]
GO
ALTER TABLE [dbo].[tbl_npc_quest_history] ADD  CONSTRAINT [DF_tbl_npc_quest_Time38]  DEFAULT ((0)) FOR [Time38]
GO
ALTER TABLE [dbo].[tbl_npc_quest_history] ADD  CONSTRAINT [DF_tbl_npc_quest_Code39]  DEFAULT ('*') FOR [Code39]
GO
ALTER TABLE [dbo].[tbl_npc_quest_history] ADD  CONSTRAINT [DF_tbl_npc_quest_Level39]  DEFAULT (0xFF) FOR [Level39]
GO
ALTER TABLE [dbo].[tbl_npc_quest_history] ADD  CONSTRAINT [DF_tbl_npc_quest_Time39]  DEFAULT ((0)) FOR [Time39]
GO
ALTER TABLE [dbo].[tbl_npc_quest_history] ADD  CONSTRAINT [DF_tbl_npc_quest_Code40]  DEFAULT ('*') FOR [Code40]
GO
ALTER TABLE [dbo].[tbl_npc_quest_history] ADD  CONSTRAINT [DF_tbl_npc_quest_Level40]  DEFAULT (0xFF) FOR [Level40]
GO
ALTER TABLE [dbo].[tbl_npc_quest_history] ADD  CONSTRAINT [DF_tbl_npc_quest_Time40]  DEFAULT ((0)) FOR [Time40]
GO
ALTER TABLE [dbo].[tbl_npc_quest_history] ADD  CONSTRAINT [DF_tbl_npc_quest_Code41]  DEFAULT ('*') FOR [Code41]
GO
ALTER TABLE [dbo].[tbl_npc_quest_history] ADD  CONSTRAINT [DF_tbl_npc_quest_Level41]  DEFAULT (0xFF) FOR [Level41]
GO
ALTER TABLE [dbo].[tbl_npc_quest_history] ADD  CONSTRAINT [DF_tbl_npc_quest_Time41]  DEFAULT ((0)) FOR [Time41]
GO
ALTER TABLE [dbo].[tbl_npc_quest_history] ADD  CONSTRAINT [DF_tbl_npc_quest_Code42]  DEFAULT ('*') FOR [Code42]
GO
ALTER TABLE [dbo].[tbl_npc_quest_history] ADD  CONSTRAINT [DF_tbl_npc_quest_Level42]  DEFAULT (0xFF) FOR [Level42]
GO
ALTER TABLE [dbo].[tbl_npc_quest_history] ADD  CONSTRAINT [DF_tbl_npc_quest_Time42]  DEFAULT ((0)) FOR [Time42]
GO
ALTER TABLE [dbo].[tbl_npc_quest_history] ADD  CONSTRAINT [DF_tbl_npc_quest_Code43]  DEFAULT ('*') FOR [Code43]
GO
ALTER TABLE [dbo].[tbl_npc_quest_history] ADD  CONSTRAINT [DF_tbl_npc_quest_Level43]  DEFAULT (0xFF) FOR [Level43]
GO
ALTER TABLE [dbo].[tbl_npc_quest_history] ADD  CONSTRAINT [DF_tbl_npc_quest_Time43]  DEFAULT ((0)) FOR [Time43]
GO
ALTER TABLE [dbo].[tbl_npc_quest_history] ADD  CONSTRAINT [DF_tbl_npc_quest_Code44]  DEFAULT ('*') FOR [Code44]
GO
ALTER TABLE [dbo].[tbl_npc_quest_history] ADD  CONSTRAINT [DF_tbl_npc_quest_Level44]  DEFAULT (0xFF) FOR [Level44]
GO
ALTER TABLE [dbo].[tbl_npc_quest_history] ADD  CONSTRAINT [DF_tbl_npc_quest_Time44]  DEFAULT ((0)) FOR [Time44]
GO
ALTER TABLE [dbo].[tbl_npc_quest_history] ADD  CONSTRAINT [DF_tbl_npc_quest_Code45]  DEFAULT ('*') FOR [Code45]
GO
ALTER TABLE [dbo].[tbl_npc_quest_history] ADD  CONSTRAINT [DF_tbl_npc_quest_Level45]  DEFAULT (0xFF) FOR [Level45]
GO
ALTER TABLE [dbo].[tbl_npc_quest_history] ADD  CONSTRAINT [DF_tbl_npc_quest_Time45]  DEFAULT ((0)) FOR [Time45]
GO
ALTER TABLE [dbo].[tbl_npc_quest_history] ADD  CONSTRAINT [DF_tbl_npc_quest_Code46]  DEFAULT ('*') FOR [Code46]
GO
ALTER TABLE [dbo].[tbl_npc_quest_history] ADD  CONSTRAINT [DF_tbl_npc_quest_Level46]  DEFAULT (0xFF) FOR [Level46]
GO
ALTER TABLE [dbo].[tbl_npc_quest_history] ADD  CONSTRAINT [DF_tbl_npc_quest_Time46]  DEFAULT ((0)) FOR [Time46]
GO
ALTER TABLE [dbo].[tbl_npc_quest_history] ADD  CONSTRAINT [DF_tbl_npc_quest_Code47]  DEFAULT ('*') FOR [Code47]
GO
ALTER TABLE [dbo].[tbl_npc_quest_history] ADD  CONSTRAINT [DF_tbl_npc_quest_Level47]  DEFAULT (0xFF) FOR [Level47]
GO
ALTER TABLE [dbo].[tbl_npc_quest_history] ADD  CONSTRAINT [DF_tbl_npc_quest_Time47]  DEFAULT ((0)) FOR [Time47]
GO
ALTER TABLE [dbo].[tbl_npc_quest_history] ADD  CONSTRAINT [DF_tbl_npc_quest_Code48]  DEFAULT ('*') FOR [Code48]
GO
ALTER TABLE [dbo].[tbl_npc_quest_history] ADD  CONSTRAINT [DF_tbl_npc_quest_Level48]  DEFAULT (0xFF) FOR [Level48]
GO
ALTER TABLE [dbo].[tbl_npc_quest_history] ADD  CONSTRAINT [DF_tbl_npc_quest_Time48]  DEFAULT ((0)) FOR [Time48]
GO
ALTER TABLE [dbo].[tbl_npc_quest_history] ADD  CONSTRAINT [DF_tbl_npc_quest_Code49]  DEFAULT ('*') FOR [Code49]
GO
ALTER TABLE [dbo].[tbl_npc_quest_history] ADD  CONSTRAINT [DF_tbl_npc_quest_Level49]  DEFAULT (0xFF) FOR [Level49]
GO
ALTER TABLE [dbo].[tbl_npc_quest_history] ADD  CONSTRAINT [DF_tbl_npc_quest_Time49]  DEFAULT ((0)) FOR [Time49]
GO
ALTER TABLE [dbo].[tbl_npc_quest_history] ADD  CONSTRAINT [DF_tbl_npc_quest_Code50]  DEFAULT ('*') FOR [Code50]
GO
ALTER TABLE [dbo].[tbl_npc_quest_history] ADD  CONSTRAINT [DF_tbl_npc_quest_Level50]  DEFAULT (0xFF) FOR [Level50]
GO
ALTER TABLE [dbo].[tbl_npc_quest_history] ADD  CONSTRAINT [DF_tbl_npc_quest_Time50]  DEFAULT ((0)) FOR [Time50]
GO
ALTER TABLE [dbo].[tbl_npc_quest_history] ADD  CONSTRAINT [DF_tbl_npc_quest_Code51]  DEFAULT ('*') FOR [Code51]
GO
ALTER TABLE [dbo].[tbl_npc_quest_history] ADD  CONSTRAINT [DF_tbl_npc_quest_Level51]  DEFAULT (0xFF) FOR [Level51]
GO
ALTER TABLE [dbo].[tbl_npc_quest_history] ADD  CONSTRAINT [DF_tbl_npc_quest_Time51]  DEFAULT ((0)) FOR [Time51]
GO
ALTER TABLE [dbo].[tbl_npc_quest_history] ADD  CONSTRAINT [DF_tbl_npc_quest_Code52]  DEFAULT ('*') FOR [Code52]
GO
ALTER TABLE [dbo].[tbl_npc_quest_history] ADD  CONSTRAINT [DF_tbl_npc_quest_Level52]  DEFAULT (0xFF) FOR [Level52]
GO
ALTER TABLE [dbo].[tbl_npc_quest_history] ADD  CONSTRAINT [DF_tbl_npc_quest_Time52]  DEFAULT ((0)) FOR [Time52]
GO
ALTER TABLE [dbo].[tbl_npc_quest_history] ADD  CONSTRAINT [DF_tbl_npc_quest_Code53]  DEFAULT ('*') FOR [Code53]
GO
ALTER TABLE [dbo].[tbl_npc_quest_history] ADD  CONSTRAINT [DF_tbl_npc_quest_Level53]  DEFAULT (0xFF) FOR [Level53]
GO
ALTER TABLE [dbo].[tbl_npc_quest_history] ADD  CONSTRAINT [DF_tbl_npc_quest_Time53]  DEFAULT ((0)) FOR [Time53]
GO
ALTER TABLE [dbo].[tbl_npc_quest_history] ADD  CONSTRAINT [DF_tbl_npc_quest_Code54]  DEFAULT ('*') FOR [Code54]
GO
ALTER TABLE [dbo].[tbl_npc_quest_history] ADD  CONSTRAINT [DF_tbl_npc_quest_Level54]  DEFAULT (0xFF) FOR [Level54]
GO
ALTER TABLE [dbo].[tbl_npc_quest_history] ADD  CONSTRAINT [DF_tbl_npc_quest_Time54]  DEFAULT ((0)) FOR [Time54]
GO
ALTER TABLE [dbo].[tbl_npc_quest_history] ADD  CONSTRAINT [DF_tbl_npc_quest_Code55]  DEFAULT ('*') FOR [Code55]
GO
ALTER TABLE [dbo].[tbl_npc_quest_history] ADD  CONSTRAINT [DF_tbl_npc_quest_Level55]  DEFAULT (0xFF) FOR [Level55]
GO
ALTER TABLE [dbo].[tbl_npc_quest_history] ADD  CONSTRAINT [DF_tbl_npc_quest_Time55]  DEFAULT ((0)) FOR [Time55]
GO
ALTER TABLE [dbo].[tbl_npc_quest_history] ADD  CONSTRAINT [DF_tbl_npc_quest_Code56]  DEFAULT ('*') FOR [Code56]
GO
ALTER TABLE [dbo].[tbl_npc_quest_history] ADD  CONSTRAINT [DF_tbl_npc_quest_Level56]  DEFAULT (0xFF) FOR [Level56]
GO
ALTER TABLE [dbo].[tbl_npc_quest_history] ADD  CONSTRAINT [DF_tbl_npc_quest_Time56]  DEFAULT ((0)) FOR [Time56]
GO
ALTER TABLE [dbo].[tbl_npc_quest_history] ADD  CONSTRAINT [DF_tbl_npc_quest_Code57]  DEFAULT ('*') FOR [Code57]
GO
ALTER TABLE [dbo].[tbl_npc_quest_history] ADD  CONSTRAINT [DF_tbl_npc_quest_Level57]  DEFAULT (0xFF) FOR [Level57]
GO
ALTER TABLE [dbo].[tbl_npc_quest_history] ADD  CONSTRAINT [DF_tbl_npc_quest_Time57]  DEFAULT ((0)) FOR [Time57]
GO
ALTER TABLE [dbo].[tbl_npc_quest_history] ADD  CONSTRAINT [DF_tbl_npc_quest_Code58]  DEFAULT ('*') FOR [Code58]
GO
ALTER TABLE [dbo].[tbl_npc_quest_history] ADD  CONSTRAINT [DF_tbl_npc_quest_Level58]  DEFAULT (0xFF) FOR [Level58]
GO
ALTER TABLE [dbo].[tbl_npc_quest_history] ADD  CONSTRAINT [DF_tbl_npc_quest_Time58]  DEFAULT ((0)) FOR [Time58]
GO
ALTER TABLE [dbo].[tbl_npc_quest_history] ADD  CONSTRAINT [DF_tbl_npc_quest_Code59]  DEFAULT ('*') FOR [Code59]
GO
ALTER TABLE [dbo].[tbl_npc_quest_history] ADD  CONSTRAINT [DF_tbl_npc_quest_Level59]  DEFAULT (0xFF) FOR [Level59]
GO
ALTER TABLE [dbo].[tbl_npc_quest_history] ADD  CONSTRAINT [DF_tbl_npc_quest_Time59]  DEFAULT ((0)) FOR [Time59]
GO
ALTER TABLE [dbo].[tbl_npc_quest_history] ADD  CONSTRAINT [DF_tbl_npc_quest_Code60]  DEFAULT ('*') FOR [Code60]
GO
ALTER TABLE [dbo].[tbl_npc_quest_history] ADD  CONSTRAINT [DF_tbl_npc_quest_Level60]  DEFAULT (0xFF) FOR [Level60]
GO
ALTER TABLE [dbo].[tbl_npc_quest_history] ADD  CONSTRAINT [DF_tbl_npc_quest_Time60]  DEFAULT ((0)) FOR [Time60]
GO
ALTER TABLE [dbo].[tbl_npc_quest_history] ADD  CONSTRAINT [DF_tbl_npc_quest_Code61]  DEFAULT ('*') FOR [Code61]
GO
ALTER TABLE [dbo].[tbl_npc_quest_history] ADD  CONSTRAINT [DF_tbl_npc_quest_Level61]  DEFAULT (0xFF) FOR [Level61]
GO
ALTER TABLE [dbo].[tbl_npc_quest_history] ADD  CONSTRAINT [DF_tbl_npc_quest_Time61]  DEFAULT ((0)) FOR [Time61]
GO
ALTER TABLE [dbo].[tbl_npc_quest_history] ADD  CONSTRAINT [DF_tbl_npc_quest_Code62]  DEFAULT ('*') FOR [Code62]
GO
ALTER TABLE [dbo].[tbl_npc_quest_history] ADD  CONSTRAINT [DF_tbl_npc_quest_Level62]  DEFAULT (0xFF) FOR [Level62]
GO
ALTER TABLE [dbo].[tbl_npc_quest_history] ADD  CONSTRAINT [DF_tbl_npc_quest_Time62]  DEFAULT ((0)) FOR [Time62]
GO
ALTER TABLE [dbo].[tbl_npc_quest_history] ADD  CONSTRAINT [DF_tbl_npc_quest_Code63]  DEFAULT ('*') FOR [Code63]
GO
ALTER TABLE [dbo].[tbl_npc_quest_history] ADD  CONSTRAINT [DF_tbl_npc_quest_Level63]  DEFAULT (0xFF) FOR [Level63]
GO
ALTER TABLE [dbo].[tbl_npc_quest_history] ADD  CONSTRAINT [DF_tbl_npc_quest_Time63]  DEFAULT ((0)) FOR [Time63]
GO
ALTER TABLE [dbo].[tbl_npc_quest_history] ADD  CONSTRAINT [DF_tbl_npc_quest_Code64]  DEFAULT ('*') FOR [Code64]
GO
ALTER TABLE [dbo].[tbl_npc_quest_history] ADD  CONSTRAINT [DF_tbl_npc_quest_Level64]  DEFAULT (0xFF) FOR [Level64]
GO
ALTER TABLE [dbo].[tbl_npc_quest_history] ADD  CONSTRAINT [DF_tbl_npc_quest_Time64]  DEFAULT ((0)) FOR [Time64]
GO
ALTER TABLE [dbo].[tbl_npc_quest_history] ADD  CONSTRAINT [DF_tbl_npc_quest_Code65]  DEFAULT ('*') FOR [Code65]
GO
ALTER TABLE [dbo].[tbl_npc_quest_history] ADD  CONSTRAINT [DF_tbl_npc_quest_Level65]  DEFAULT (0xFF) FOR [Level65]
GO
ALTER TABLE [dbo].[tbl_npc_quest_history] ADD  CONSTRAINT [DF_tbl_npc_quest_Time65]  DEFAULT ((0)) FOR [Time65]
GO
ALTER TABLE [dbo].[tbl_npc_quest_history] ADD  CONSTRAINT [DF_tbl_npc_quest_Code66]  DEFAULT ('*') FOR [Code66]
GO
ALTER TABLE [dbo].[tbl_npc_quest_history] ADD  CONSTRAINT [DF_tbl_npc_quest_Level66]  DEFAULT (0xFF) FOR [Level66]
GO
ALTER TABLE [dbo].[tbl_npc_quest_history] ADD  CONSTRAINT [DF_tbl_npc_quest_Time66]  DEFAULT ((0)) FOR [Time66]
GO
ALTER TABLE [dbo].[tbl_npc_quest_history] ADD  CONSTRAINT [DF_tbl_npc_quest_Code67]  DEFAULT ('*') FOR [Code67]
GO
ALTER TABLE [dbo].[tbl_npc_quest_history] ADD  CONSTRAINT [DF_tbl_npc_quest_Level67]  DEFAULT (0xFF) FOR [Level67]
GO
ALTER TABLE [dbo].[tbl_npc_quest_history] ADD  CONSTRAINT [DF_tbl_npc_quest_Time67]  DEFAULT ((0)) FOR [Time67]
GO
ALTER TABLE [dbo].[tbl_npc_quest_history] ADD  CONSTRAINT [DF_tbl_npc_quest_Code68]  DEFAULT ('*') FOR [Code68]
GO
ALTER TABLE [dbo].[tbl_npc_quest_history] ADD  CONSTRAINT [DF_tbl_npc_quest_Level68]  DEFAULT (0xFF) FOR [Level68]
GO
ALTER TABLE [dbo].[tbl_npc_quest_history] ADD  CONSTRAINT [DF_tbl_npc_quest_Time68]  DEFAULT ((0)) FOR [Time68]
GO
ALTER TABLE [dbo].[tbl_npc_quest_history] ADD  CONSTRAINT [DF_tbl_npc_quest_Code69]  DEFAULT ('*') FOR [Code69]
GO
ALTER TABLE [dbo].[tbl_npc_quest_history] ADD  CONSTRAINT [DF_tbl_npc_quest_Level69]  DEFAULT (0xFF) FOR [Level69]
GO
ALTER TABLE [dbo].[tbl_npc_quest_history] ADD  CONSTRAINT [DF_tbl_npc_quest_Time69]  DEFAULT ((0)) FOR [Time69]
GO
ALTER TABLE [dbo].[tbl_npc_quest_history] ADD  CONSTRAINT [DF_tbl_npc_quest_Code70]  DEFAULT ('*') FOR [Code70]
GO
ALTER TABLE [dbo].[tbl_npc_quest_history] ADD  CONSTRAINT [DF_tbl_npc_quest_Level70]  DEFAULT (0xFF) FOR [Level70]
GO
ALTER TABLE [dbo].[tbl_npc_quest_history] ADD  CONSTRAINT [DF_tbl_npc_quest_Time70]  DEFAULT ((0)) FOR [Time70]
GO
ALTER TABLE [dbo].[tbl_NpcData] ADD  CONSTRAINT [DF_tbl_NpcData_Serial]  DEFAULT (0xFFFFFFFF) FOR [Serial]
GO
ALTER TABLE [dbo].[tbl_NpcData] ADD  CONSTRAINT [DF_tbl_NpcData_Npc0]  DEFAULT ((0)) FOR [Npc0]
GO
ALTER TABLE [dbo].[tbl_NpcData] ADD  CONSTRAINT [DF_tbl_NpcData_Npc1]  DEFAULT ((0)) FOR [Npc1]
GO
ALTER TABLE [dbo].[tbl_NpcData] ADD  CONSTRAINT [DF_tbl_NpcData_Npc2]  DEFAULT ((0)) FOR [Npc2]
GO
ALTER TABLE [dbo].[tbl_NpcData] ADD  CONSTRAINT [DF_tbl_NpcData_Npc3]  DEFAULT ((0)) FOR [Npc3]
GO
ALTER TABLE [dbo].[tbl_NpcData] ADD  CONSTRAINT [DF_tbl_NpcData_Npc4]  DEFAULT ((0)) FOR [Npc4]
GO
ALTER TABLE [dbo].[tbl_NpcData] ADD  CONSTRAINT [DF_tbl_NpcData_Npc5]  DEFAULT ((0)) FOR [Npc5]
GO
ALTER TABLE [dbo].[tbl_NpcData] ADD  CONSTRAINT [DF_tbl_NpcData_Npc6]  DEFAULT ((0)) FOR [Npc6]
GO
ALTER TABLE [dbo].[tbl_NpcData] ADD  CONSTRAINT [DF_tbl_NpcData_Npc7]  DEFAULT ((0)) FOR [Npc7]
GO
ALTER TABLE [dbo].[tbl_NpcData] ADD  CONSTRAINT [DF_tbl_NpcData_Npc8]  DEFAULT ((0)) FOR [Npc8]
GO
ALTER TABLE [dbo].[tbl_NpcData] ADD  CONSTRAINT [DF_tbl_NpcData_Npc9]  DEFAULT ((0)) FOR [Npc9]
GO
ALTER TABLE [dbo].[tbl_NpcLog] ADD  CONSTRAINT [DF_tbl_NpcLog_CharSerial]  DEFAULT (0xFFFFFFFF) FOR [CharSerial]
GO
ALTER TABLE [dbo].[tbl_NpcLog] ADD  CONSTRAINT [DF_tbl_NpcLog_Name]  DEFAULT ('*') FOR [Name]
GO
ALTER TABLE [dbo].[tbl_NpcLog] ADD  CONSTRAINT [DF_tbl_NpcLog_LogDate]  DEFAULT (getdate()) FOR [LogDate]
GO
ALTER TABLE [dbo].[tbl_OreCutting] ADD  CONSTRAINT [DF__tbl_OreCutting_DCK]  DEFAULT ((0)) FOR [DCK]
GO
ALTER TABLE [dbo].[tbl_OreCutting] ADD  CONSTRAINT [DF__tbl_OreCutting_K0]  DEFAULT (0xFFFFFFFF) FOR [K0]
GO
ALTER TABLE [dbo].[tbl_OreCutting] ADD  CONSTRAINT [DF__tbl_OreCutting_D0]  DEFAULT ((0)) FOR [D0]
GO
ALTER TABLE [dbo].[tbl_OreCutting] ADD  CONSTRAINT [DF__tbl_OreCutting_K1]  DEFAULT (0xFFFFFFFF) FOR [K1]
GO
ALTER TABLE [dbo].[tbl_OreCutting] ADD  CONSTRAINT [DF__tbl_OreCutting_D1]  DEFAULT ((0)) FOR [D1]
GO
ALTER TABLE [dbo].[tbl_OreCutting] ADD  CONSTRAINT [DF__tbl_OreCutting_K2]  DEFAULT (0xFFFFFFFF) FOR [K2]
GO
ALTER TABLE [dbo].[tbl_OreCutting] ADD  CONSTRAINT [DF__tbl_OreCutting_D2]  DEFAULT ((0)) FOR [D2]
GO
ALTER TABLE [dbo].[tbl_OreCutting] ADD  CONSTRAINT [DF__tbl_OreCutting_K3]  DEFAULT (0xFFFFFFFF) FOR [K3]
GO
ALTER TABLE [dbo].[tbl_OreCutting] ADD  CONSTRAINT [DF__tbl_OreCutting_D3]  DEFAULT ((0)) FOR [D3]
GO
ALTER TABLE [dbo].[tbl_OreCutting] ADD  CONSTRAINT [DF__tbl_OreCutting_K4]  DEFAULT (0xFFFFFFFF) FOR [K4]
GO
ALTER TABLE [dbo].[tbl_OreCutting] ADD  CONSTRAINT [DF__tbl_OreCutting_D4]  DEFAULT ((0)) FOR [D4]
GO
ALTER TABLE [dbo].[tbl_OreCutting] ADD  CONSTRAINT [DF__tbl_OreCutting_K5]  DEFAULT (0xFFFFFFFF) FOR [K5]
GO
ALTER TABLE [dbo].[tbl_OreCutting] ADD  CONSTRAINT [DF__tbl_OreCutting_D5]  DEFAULT ((0)) FOR [D5]
GO
ALTER TABLE [dbo].[tbl_OreCutting] ADD  CONSTRAINT [DF__tbl_OreCutting_K6]  DEFAULT (0xFFFFFFFF) FOR [K6]
GO
ALTER TABLE [dbo].[tbl_OreCutting] ADD  CONSTRAINT [DF__tbl_OreCutting_D6]  DEFAULT ((0)) FOR [D6]
GO
ALTER TABLE [dbo].[tbl_OreCutting] ADD  CONSTRAINT [DF__tbl_OreCutting_K7]  DEFAULT (0xFFFFFFFF) FOR [K7]
GO
ALTER TABLE [dbo].[tbl_OreCutting] ADD  CONSTRAINT [DF__tbl_OreCutting_D7]  DEFAULT ((0)) FOR [D7]
GO
ALTER TABLE [dbo].[tbl_OreCutting] ADD  CONSTRAINT [DF__tbl_OreCutting_K8]  DEFAULT (0xFFFFFFFF) FOR [K8]
GO
ALTER TABLE [dbo].[tbl_OreCutting] ADD  CONSTRAINT [DF__tbl_OreCutting_D8]  DEFAULT ((0)) FOR [D8]
GO
ALTER TABLE [dbo].[tbl_OreCutting] ADD  CONSTRAINT [DF__tbl_OreCutting_K9]  DEFAULT (0xFFFFFFFF) FOR [K9]
GO
ALTER TABLE [dbo].[tbl_OreCutting] ADD  CONSTRAINT [DF__tbl_OreCutting_D9]  DEFAULT ((0)) FOR [D9]
GO
ALTER TABLE [dbo].[tbl_OreCutting] ADD  CONSTRAINT [DF__tbl_OreCutting_K10]  DEFAULT (0xFFFFFFFF) FOR [K10]
GO
ALTER TABLE [dbo].[tbl_OreCutting] ADD  CONSTRAINT [DF__tbl_OreCutting_D10]  DEFAULT ((0)) FOR [D10]
GO
ALTER TABLE [dbo].[tbl_OreCutting] ADD  CONSTRAINT [DF__tbl_OreCutting_K11]  DEFAULT (0xFFFFFFFF) FOR [K11]
GO
ALTER TABLE [dbo].[tbl_OreCutting] ADD  CONSTRAINT [DF__tbl_OreCutting_D11]  DEFAULT ((0)) FOR [D11]
GO
ALTER TABLE [dbo].[tbl_OreCutting] ADD  CONSTRAINT [DF__tbl_OreCutting_K12]  DEFAULT (0xFFFFFFFF) FOR [K12]
GO
ALTER TABLE [dbo].[tbl_OreCutting] ADD  CONSTRAINT [DF__tbl_OreCutting_D12]  DEFAULT ((0)) FOR [D12]
GO
ALTER TABLE [dbo].[tbl_OreCutting] ADD  CONSTRAINT [DF__tbl_OreCutting_K13]  DEFAULT (0xFFFFFFFF) FOR [K13]
GO
ALTER TABLE [dbo].[tbl_OreCutting] ADD  CONSTRAINT [DF__tbl_OreCutting_D13]  DEFAULT ((0)) FOR [D13]
GO
ALTER TABLE [dbo].[tbl_OreCutting] ADD  CONSTRAINT [DF__tbl_OreCutting_K14]  DEFAULT (0xFFFFFFFF) FOR [K14]
GO
ALTER TABLE [dbo].[tbl_OreCutting] ADD  CONSTRAINT [DF__tbl_OreCutting_D14]  DEFAULT ((0)) FOR [D14]
GO
ALTER TABLE [dbo].[tbl_OreCutting] ADD  CONSTRAINT [DF__tbl_OreCutting_K15]  DEFAULT (0xFFFFFFFF) FOR [K15]
GO
ALTER TABLE [dbo].[tbl_OreCutting] ADD  CONSTRAINT [DF__tbl_OreCutting_D15]  DEFAULT ((0)) FOR [D15]
GO
ALTER TABLE [dbo].[tbl_OreCutting] ADD  CONSTRAINT [DF__tbl_OreCutting_K16]  DEFAULT (0xFFFFFFFF) FOR [K16]
GO
ALTER TABLE [dbo].[tbl_OreCutting] ADD  CONSTRAINT [DF__tbl_OreCutting_D16]  DEFAULT ((0)) FOR [D16]
GO
ALTER TABLE [dbo].[tbl_OreCutting] ADD  CONSTRAINT [DF__tbl_OreCutting_K17]  DEFAULT (0xFFFFFFFF) FOR [K17]
GO
ALTER TABLE [dbo].[tbl_OreCutting] ADD  CONSTRAINT [DF__tbl_OreCutting_D17]  DEFAULT ((0)) FOR [D17]
GO
ALTER TABLE [dbo].[tbl_OreCutting] ADD  CONSTRAINT [DF__tbl_OreCutting_K18]  DEFAULT (0xFFFFFFFF) FOR [K18]
GO
ALTER TABLE [dbo].[tbl_OreCutting] ADD  CONSTRAINT [DF__tbl_OreCutting_D18]  DEFAULT ((0)) FOR [D18]
GO
ALTER TABLE [dbo].[tbl_OreCutting] ADD  CONSTRAINT [DF__tbl_OreCutting_K19]  DEFAULT (0xFFFFFFFF) FOR [K19]
GO
ALTER TABLE [dbo].[tbl_OreCutting] ADD  CONSTRAINT [DF__tbl_OreCutting_D19]  DEFAULT ((0)) FOR [D19]
GO
ALTER TABLE [dbo].[tbl_patriarch_candidate] ADD  CONSTRAINT [DF__tbl_patri__eSeri__176E7E35]  DEFAULT ((0)) FOR [eSerial]
GO
ALTER TABLE [dbo].[tbl_patriarch_candidate] ADD  CONSTRAINT [DF__tbl_patria__Race__1862A26E]  DEFAULT (0xFF) FOR [Race]
GO
ALTER TABLE [dbo].[tbl_patriarch_candidate] ADD  CONSTRAINT [DF__tbl_patriarc__Lv__1956C6A7]  DEFAULT (0xFF) FOR [Lv]
GO
ALTER TABLE [dbo].[tbl_patriarch_candidate] ADD  CONSTRAINT [DF__tbl_patria__Rank__1A4AEAE0]  DEFAULT ((0)) FOR [Rank]
GO
ALTER TABLE [dbo].[tbl_patriarch_candidate] ADD  CONSTRAINT [DF__tbl_patri__Grade__1B3F0F19]  DEFAULT ((7)) FOR [Grade]
GO
ALTER TABLE [dbo].[tbl_patriarch_candidate] ADD  CONSTRAINT [DF__tbl_patri__PvpPo__1C333352]  DEFAULT ((0.0)) FOR [PvpPoint]
GO
ALTER TABLE [dbo].[tbl_patriarch_candidate] ADD  CONSTRAINT [DF__tbl_patri__ASeri__1D27578B]  DEFAULT (0xFFFFFFFF) FOR [ASerial]
GO
ALTER TABLE [dbo].[tbl_patriarch_candidate] ADD  CONSTRAINT [DF__tbl_patri__AName__1E1B7BC4]  DEFAULT ('*') FOR [AName]
GO
ALTER TABLE [dbo].[tbl_patriarch_candidate] ADD  CONSTRAINT [DF__tbl_patri__GSeri__1F0F9FFD]  DEFAULT (0xFFFFFFFF) FOR [GSerial]
GO
ALTER TABLE [dbo].[tbl_patriarch_candidate] ADD  CONSTRAINT [DF__tbl_patri__GName__2003C436]  DEFAULT ('*') FOR [GName]
GO
ALTER TABLE [dbo].[tbl_patriarch_candidate] ADD  CONSTRAINT [DF__tbl_patri__WinCn__20F7E86F]  DEFAULT ((0)) FOR [WinCnt]
GO
ALTER TABLE [dbo].[tbl_patriarch_candidate] ADD  CONSTRAINT [DF__tbl_patri__Class__21EC0CA8]  DEFAULT (0xFF) FOR [ClassType]
GO
ALTER TABLE [dbo].[tbl_patriarch_candidate] ADD  CONSTRAINT [DF__tbl_patri__State__22E030E1]  DEFAULT ((0)) FOR [State]
GO
ALTER TABLE [dbo].[tbl_patriarch_candidate] ADD  CONSTRAINT [DF__tbl_patri__Score__23D4551A]  DEFAULT ((0)) FOR [Score]
GO
ALTER TABLE [dbo].[tbl_patriarch_candidate] ADD  CONSTRAINT [DF__tbl_patri__Refun__24C87953]  DEFAULT ((0)) FOR [Refund]
GO
ALTER TABLE [dbo].[tbl_patriarch_candidate] ADD  CONSTRAINT [DF__tbl_patri__dtUpS__25BC9D8C]  DEFAULT ('1900-01-01') FOR [dtUpScore]
GO
ALTER TABLE [dbo].[tbl_patriarch_comm] ADD  CONSTRAINT [DF__tbl_patri__PayDa__7EA2D06B]  DEFAULT ('1990-01-01') FOR [PayDate]
GO
ALTER TABLE [dbo].[tbl_patriarch_comm] ADD  CONSTRAINT [DF__tbl_patriar__DCK__7F96F4A4]  DEFAULT ((0)) FOR [DCK]
GO
ALTER TABLE [dbo].[tbl_patriarch_elect] ADD  CONSTRAINT [DF__tbl_patri__World__0B08A750]  DEFAULT ('*') FOR [WorldName]
GO
ALTER TABLE [dbo].[tbl_patriarch_elect] ADD  CONSTRAINT [DF__tbl_patri__ProcT__0BFCCB89]  DEFAULT (0xFF) FOR [ProcType]
GO
ALTER TABLE [dbo].[tbl_patriarch_elect] ADD  CONSTRAINT [DF__tbl_patri__NonVo__0CF0EFC2]  DEFAULT ((0)) FOR [NonVoteCnt_0]
GO
ALTER TABLE [dbo].[tbl_patriarch_elect] ADD  CONSTRAINT [DF__tbl_patri__NonVo__0DE513FB]  DEFAULT ((0)) FOR [NonVoteCnt_1]
GO
ALTER TABLE [dbo].[tbl_patriarch_elect] ADD  CONSTRAINT [DF__tbl_patri__NonVo__0ED93834]  DEFAULT ((0)) FOR [NonVoteCnt_2]
GO
ALTER TABLE [dbo].[tbl_patriarch_elect] ADD  CONSTRAINT [DF__tbl_patri__VoteC__0FCD5C6D]  DEFAULT ((0)) FOR [VoteCnt_0]
GO
ALTER TABLE [dbo].[tbl_patriarch_elect] ADD  CONSTRAINT [DF__tbl_patri__VoteC__10C180A6]  DEFAULT ((0)) FOR [VoteCnt_1]
GO
ALTER TABLE [dbo].[tbl_patriarch_elect] ADD  CONSTRAINT [DF__tbl_patri__VoteC__11B5A4DF]  DEFAULT ((0)) FOR [VoteCnt_2]
GO
ALTER TABLE [dbo].[tbl_patriarch_elect] ADD  CONSTRAINT [DF__tbl_patri__tmCre__12A9C918]  DEFAULT (getdate()) FOR [tmCreate]
GO
ALTER TABLE [dbo].[tbl_patriarch_elect] ADD  CONSTRAINT [DF__tbl_patri__tmUpd__139DED51]  DEFAULT (getdate()) FOR [tmUpdate]
GO
ALTER TABLE [dbo].[tbl_patriarch_elect] ADD  CONSTRAINT [DF__tbl_patriarch_elect_HighGradeNum_0]  DEFAULT ((0)) FOR [HighGradeNum_0]
GO
ALTER TABLE [dbo].[tbl_patriarch_elect] ADD  CONSTRAINT [DF__tbl_patriarch_elect_HighGradeNum_1]  DEFAULT ((0)) FOR [HighGradeNum_1]
GO
ALTER TABLE [dbo].[tbl_patriarch_elect] ADD  CONSTRAINT [DF__tbl_patriarch_elect_HighGradeNum_2]  DEFAULT ((0)) FOR [HighGradeNum_2]
GO
ALTER TABLE [dbo].[tbl_patriarch_elect] ADD  CONSTRAINT [DF_tbl_patriarch_elect_VoteInfoResetTime]  DEFAULT ((1990011)) FOR [VoteInfoResetTime]
GO
ALTER TABLE [dbo].[tbl_pcbangitem] ADD  CONSTRAINT [DF_tbl_pcbangitem_Serial]  DEFAULT ((-1)) FOR [Serial]
GO
ALTER TABLE [dbo].[tbl_pcbangitem] ADD  CONSTRAINT [DF_tbl_pcbangitem_DCK]  DEFAULT ((0)) FOR [DCK]
GO
ALTER TABLE [dbo].[tbl_pcbangitem] ADD  CONSTRAINT [DF_tbl_pcbangitem_k0]  DEFAULT ((-1)) FOR [K0]
GO
ALTER TABLE [dbo].[tbl_pcbangitem] ADD  CONSTRAINT [DF_tbl_pcbangitem_k1]  DEFAULT ((-1)) FOR [K1]
GO
ALTER TABLE [dbo].[tbl_pcbangitem] ADD  CONSTRAINT [DF_tbl_pcbangitem_k2]  DEFAULT ((-1)) FOR [K2]
GO
ALTER TABLE [dbo].[tbl_pcbangitem] ADD  CONSTRAINT [DF_tbl_pcbangitem_k3]  DEFAULT ((-1)) FOR [K3]
GO
ALTER TABLE [dbo].[tbl_pcbangitem] ADD  CONSTRAINT [DF_tbl_pcbangitem_k4]  DEFAULT ((-1)) FOR [K4]
GO
ALTER TABLE [dbo].[tbl_pcbangitem] ADD  CONSTRAINT [DF_tbl_pcbangitem_k5]  DEFAULT ((-1)) FOR [K5]
GO
ALTER TABLE [dbo].[tbl_pcbangitem] ADD  CONSTRAINT [DF_tbl_pcbangitem_k6]  DEFAULT ((-1)) FOR [K6]
GO
ALTER TABLE [dbo].[tbl_pcbangitem] ADD  CONSTRAINT [DF_tbl_pcbangitem_k7]  DEFAULT ((-1)) FOR [K7]
GO
ALTER TABLE [dbo].[tbl_pcbangitem] ADD  CONSTRAINT [DF_tbl_pcbangitem_k8]  DEFAULT ((-1)) FOR [K8]
GO
ALTER TABLE [dbo].[tbl_pcbangitem] ADD  CONSTRAINT [DF_tbl_pcbangitem_k9]  DEFAULT ((-1)) FOR [K9]
GO
ALTER TABLE [dbo].[tbl_pcbangitem] ADD  CONSTRAINT [DF_tbl_pcbangitem_k10]  DEFAULT ((-1)) FOR [K10]
GO
ALTER TABLE [dbo].[tbl_pcbangitem] ADD  CONSTRAINT [DF_tbl_pcbangitem_k11]  DEFAULT ((-1)) FOR [K11]
GO
ALTER TABLE [dbo].[tbl_pcbangitem] ADD  CONSTRAINT [DF_tbl_pcbangitem_k12]  DEFAULT ((-1)) FOR [K12]
GO
ALTER TABLE [dbo].[tbl_pcbangitem] ADD  CONSTRAINT [DF_tbl_pcbangitem_k13]  DEFAULT ((-1)) FOR [K13]
GO
ALTER TABLE [dbo].[tbl_pcbangitem] ADD  CONSTRAINT [DF_tbl_pcbangitem_k14]  DEFAULT ((-1)) FOR [K14]
GO
ALTER TABLE [dbo].[tbl_pcbangitem] ADD  CONSTRAINT [DF_tbl_pcbangitem_k15]  DEFAULT ((-1)) FOR [K15]
GO
ALTER TABLE [dbo].[tbl_pcbangitem] ADD  CONSTRAINT [DF_tbl_pcbangitem_k16]  DEFAULT ((-1)) FOR [K16]
GO
ALTER TABLE [dbo].[tbl_pcbangitem] ADD  CONSTRAINT [DF_tbl_pcbangitem_k17]  DEFAULT ((-1)) FOR [K17]
GO
ALTER TABLE [dbo].[tbl_pcbangitem] ADD  CONSTRAINT [DF_tbl_pcbangitem_k18]  DEFAULT ((-1)) FOR [K18]
GO
ALTER TABLE [dbo].[tbl_pcbangitem] ADD  CONSTRAINT [DF_tbl_pcbangitem_k19]  DEFAULT ((-1)) FOR [K19]
GO
ALTER TABLE [dbo].[tbl_pcbangitem] ADD  CONSTRAINT [DF_tbl_pcbangitem_k20]  DEFAULT ((-1)) FOR [K20]
GO
ALTER TABLE [dbo].[tbl_pcbangitem] ADD  CONSTRAINT [DF_tbl_pcbangitem_k21]  DEFAULT ((-1)) FOR [K21]
GO
ALTER TABLE [dbo].[tbl_pcbangitem] ADD  CONSTRAINT [DF_tbl_pcbangitem_k22]  DEFAULT ((-1)) FOR [K22]
GO
ALTER TABLE [dbo].[tbl_pcbangitem] ADD  CONSTRAINT [DF_tbl_pcbangitem_k23]  DEFAULT ((-1)) FOR [K23]
GO
ALTER TABLE [dbo].[tbl_pcbangitem] ADD  CONSTRAINT [DF_tbl_pcbangitem_k24]  DEFAULT ((-1)) FOR [K24]
GO
ALTER TABLE [dbo].[tbl_pcbangitem] ADD  CONSTRAINT [DF_tbl_pcbangitem_k25]  DEFAULT ((-1)) FOR [K25]
GO
ALTER TABLE [dbo].[tbl_pcbangitem] ADD  CONSTRAINT [DF_tbl_pcbangitem_k26]  DEFAULT ((-1)) FOR [K26]
GO
ALTER TABLE [dbo].[tbl_pcbangitem] ADD  CONSTRAINT [DF_tbl_pcbangitem_k27]  DEFAULT ((-1)) FOR [K27]
GO
ALTER TABLE [dbo].[tbl_pcbangitem] ADD  CONSTRAINT [DF_tbl_pcbangitem_k28]  DEFAULT ((-1)) FOR [K28]
GO
ALTER TABLE [dbo].[tbl_pcbangitem] ADD  CONSTRAINT [DF_tbl_pcbangitem_k29]  DEFAULT ((-1)) FOR [K29]
GO
ALTER TABLE [dbo].[tbl_pcbangitem] ADD  CONSTRAINT [DF_tbl_pcbangitem_k30]  DEFAULT ((-1)) FOR [K30]
GO
ALTER TABLE [dbo].[tbl_pcbangitem] ADD  CONSTRAINT [DF_tbl_pcbangitem_k31]  DEFAULT ((-1)) FOR [K31]
GO
ALTER TABLE [dbo].[tbl_pcbangitem] ADD  CONSTRAINT [DF_tbl_pcbangitem_k32]  DEFAULT ((-1)) FOR [K32]
GO
ALTER TABLE [dbo].[tbl_pcbangitem] ADD  CONSTRAINT [DF_tbl_pcbangitem_k33]  DEFAULT ((-1)) FOR [K33]
GO
ALTER TABLE [dbo].[tbl_pcbangitem] ADD  CONSTRAINT [DF_tbl_pcbangitem_k34]  DEFAULT ((-1)) FOR [K34]
GO
ALTER TABLE [dbo].[tbl_pcbangitem] ADD  CONSTRAINT [DF_tbl_pcbangitem_k35]  DEFAULT ((-1)) FOR [K35]
GO
ALTER TABLE [dbo].[tbl_pcbangitem] ADD  CONSTRAINT [DF_tbl_pcbangitem_k36]  DEFAULT ((-1)) FOR [K36]
GO
ALTER TABLE [dbo].[tbl_pcbangitem] ADD  CONSTRAINT [DF_tbl_pcbangitem_k37]  DEFAULT ((-1)) FOR [K37]
GO
ALTER TABLE [dbo].[tbl_pcbangitem] ADD  CONSTRAINT [DF_tbl_pcbangitem_k38]  DEFAULT ((-1)) FOR [K38]
GO
ALTER TABLE [dbo].[tbl_pcbangitem] ADD  CONSTRAINT [DF_tbl_pcbangitem_k39]  DEFAULT ((-1)) FOR [K39]
GO
ALTER TABLE [dbo].[tbl_pcbangitem] ADD  CONSTRAINT [DF_tbl_pcbangitem_k40]  DEFAULT ((-1)) FOR [K40]
GO
ALTER TABLE [dbo].[tbl_pcbangitem] ADD  CONSTRAINT [DF_tbl_pcbangitem_k41]  DEFAULT ((-1)) FOR [K41]
GO
ALTER TABLE [dbo].[tbl_pcbangitem] ADD  CONSTRAINT [DF_tbl_pcbangitem_k42]  DEFAULT ((-1)) FOR [K42]
GO
ALTER TABLE [dbo].[tbl_pcbangitem] ADD  CONSTRAINT [DF_tbl_pcbangitem_k43]  DEFAULT ((-1)) FOR [K43]
GO
ALTER TABLE [dbo].[tbl_pcbangitem] ADD  CONSTRAINT [DF_tbl_pcbangitem_k44]  DEFAULT ((-1)) FOR [K44]
GO
ALTER TABLE [dbo].[tbl_pcbangitem] ADD  CONSTRAINT [DF_tbl_pcbangitem_k45]  DEFAULT ((-1)) FOR [K45]
GO
ALTER TABLE [dbo].[tbl_pcbangitem] ADD  CONSTRAINT [DF_tbl_pcbangitem_k46]  DEFAULT ((-1)) FOR [K46]
GO
ALTER TABLE [dbo].[tbl_pcbangitem] ADD  CONSTRAINT [DF_tbl_pcbangitem_k47]  DEFAULT ((-1)) FOR [K47]
GO
ALTER TABLE [dbo].[tbl_pcbangitem] ADD  CONSTRAINT [DF_tbl_pcbangitem_k48]  DEFAULT ((-1)) FOR [K48]
GO
ALTER TABLE [dbo].[tbl_pcbangitem] ADD  CONSTRAINT [DF_tbl_pcbangitem_k49]  DEFAULT ((-1)) FOR [K49]
GO
ALTER TABLE [dbo].[tbl_PostRegistry] ADD  CONSTRAINT [DF__tbl_PostR__sendr__5EF1ED8C]  DEFAULT ((0)) FOR [sendrace]
GO
ALTER TABLE [dbo].[tbl_PostRegistry] ADD  CONSTRAINT [DF__tbl_PostR__userd__5FE611C5]  DEFAULT ((0)) FOR [userdgr]
GO
ALTER TABLE [dbo].[tbl_PostRegistry] ADD  CONSTRAINT [DF__tbl_PostReg__uid__3F10C48D]  DEFAULT ((0)) FOR [uid]
GO
ALTER TABLE [dbo].[tbl_PostStorage] ADD  CONSTRAINT [DF__tbl_PostSto__dck__62C27E70]  DEFAULT ((1)) FOR [dck]
GO
ALTER TABLE [dbo].[tbl_PostStorage] ADD  CONSTRAINT [DF__tbl_PostSto__err__63B6A2A9]  DEFAULT ((0)) FOR [err]
GO
ALTER TABLE [dbo].[tbl_PostStorage] ADD  CONSTRAINT [DF__tbl_PostSto__uid__4004E8C6]  DEFAULT ((0)) FOR [uid]
GO
ALTER TABLE [dbo].[tbl_PostStorage] ADD  CONSTRAINT [DF__tbl_PostS__sinde__74E390B0]  DEFAULT (0xFF) FOR [sindex]
GO
ALTER TABLE [dbo].[tbl_potion_delay] ADD  CONSTRAINT [DF__tbl_potion_delay_DCK]  DEFAULT ((0)) FOR [DCK]
GO
ALTER TABLE [dbo].[tbl_potion_delay] ADD  CONSTRAINT [DF__tbl_potion_delay_PD0]  DEFAULT ((0)) FOR [PD0]
GO
ALTER TABLE [dbo].[tbl_potion_delay] ADD  CONSTRAINT [DF__tbl_potion_delay_PD1]  DEFAULT ((0)) FOR [PD1]
GO
ALTER TABLE [dbo].[tbl_potion_delay] ADD  CONSTRAINT [DF__tbl_potion_delay_PD2]  DEFAULT ((0)) FOR [PD2]
GO
ALTER TABLE [dbo].[tbl_potion_delay] ADD  CONSTRAINT [DF__tbl_potion_delay_PD3]  DEFAULT ((0)) FOR [PD3]
GO
ALTER TABLE [dbo].[tbl_potion_delay] ADD  CONSTRAINT [DF__tbl_potion_delay_PD4]  DEFAULT ((0)) FOR [PD4]
GO
ALTER TABLE [dbo].[tbl_potion_delay] ADD  CONSTRAINT [DF__tbl_potion_delay_PD5]  DEFAULT ((0)) FOR [PD5]
GO
ALTER TABLE [dbo].[tbl_potion_delay] ADD  CONSTRAINT [DF__tbl_potion_delay_PD6]  DEFAULT ((0)) FOR [PD6]
GO
ALTER TABLE [dbo].[tbl_potion_delay] ADD  CONSTRAINT [DF__tbl_potion_delay_PD7]  DEFAULT ((0)) FOR [PD7]
GO
ALTER TABLE [dbo].[tbl_potion_delay] ADD  CONSTRAINT [DF__tbl_potion_delay_PD8]  DEFAULT ((0)) FOR [PD8]
GO
ALTER TABLE [dbo].[tbl_potion_delay] ADD  CONSTRAINT [DF__tbl_potion_delay_PD9]  DEFAULT ((0)) FOR [PD9]
GO
ALTER TABLE [dbo].[tbl_potion_delay] ADD  CONSTRAINT [DF__tbl_potion_delay_PD10]  DEFAULT ((0)) FOR [PD10]
GO
ALTER TABLE [dbo].[tbl_potion_delay] ADD  CONSTRAINT [DF__tbl_potion_delay_PD11]  DEFAULT ((0)) FOR [PD11]
GO
ALTER TABLE [dbo].[tbl_potion_delay] ADD  CONSTRAINT [DF__tbl_potion_delay_PD12]  DEFAULT ((0)) FOR [PD12]
GO
ALTER TABLE [dbo].[tbl_potion_delay] ADD  CONSTRAINT [DF__tbl_potion_delay_PD13]  DEFAULT ((0)) FOR [PD13]
GO
ALTER TABLE [dbo].[tbl_potion_delay] ADD  CONSTRAINT [DF__tbl_potion_delay_PD14]  DEFAULT ((0)) FOR [PD14]
GO
ALTER TABLE [dbo].[tbl_potion_delay] ADD  CONSTRAINT [DF__tbl_potion_delay_PD15]  DEFAULT ((0)) FOR [PD15]
GO
ALTER TABLE [dbo].[tbl_potion_delay] ADD  CONSTRAINT [DF__tbl_potion_delay_PD16]  DEFAULT ((0)) FOR [PD16]
GO
ALTER TABLE [dbo].[tbl_potion_delay] ADD  CONSTRAINT [DF__tbl_potion_delay_PD17]  DEFAULT ((0)) FOR [PD17]
GO
ALTER TABLE [dbo].[tbl_potion_delay] ADD  CONSTRAINT [DF__tbl_potion_delay_PD18]  DEFAULT ((0)) FOR [PD18]
GO
ALTER TABLE [dbo].[tbl_potion_delay] ADD  CONSTRAINT [DF__tbl_potion_delay_PD19]  DEFAULT ((0)) FOR [PD19]
GO
ALTER TABLE [dbo].[tbl_potion_delay] ADD  CONSTRAINT [DF__tbl_potion_delay_PD20]  DEFAULT ((0)) FOR [PD20]
GO
ALTER TABLE [dbo].[tbl_potion_delay] ADD  CONSTRAINT [DF__tbl_potion_delay_PD21]  DEFAULT ((0)) FOR [PD21]
GO
ALTER TABLE [dbo].[tbl_potion_delay] ADD  CONSTRAINT [DF__tbl_potion_delay_PD22]  DEFAULT ((0)) FOR [PD22]
GO
ALTER TABLE [dbo].[tbl_potion_delay] ADD  CONSTRAINT [DF__tbl_potion_delay_PD23]  DEFAULT ((0)) FOR [PD23]
GO
ALTER TABLE [dbo].[tbl_potion_delay] ADD  CONSTRAINT [DF__tbl_potion_delay_PD24]  DEFAULT ((0)) FOR [PD24]
GO
ALTER TABLE [dbo].[tbl_potion_delay] ADD  CONSTRAINT [DF__tbl_potion_delay_PD25]  DEFAULT ((0)) FOR [PD25]
GO
ALTER TABLE [dbo].[tbl_potion_delay] ADD  CONSTRAINT [DF__tbl_potion_delay_PD26]  DEFAULT ((0)) FOR [PD26]
GO
ALTER TABLE [dbo].[tbl_potion_delay] ADD  CONSTRAINT [DF__tbl_potion_delay_PD27]  DEFAULT ((0)) FOR [PD27]
GO
ALTER TABLE [dbo].[tbl_potion_delay] ADD  CONSTRAINT [DF__tbl_potion_delay_PD28]  DEFAULT ((0)) FOR [PD28]
GO
ALTER TABLE [dbo].[tbl_potion_delay] ADD  CONSTRAINT [DF_tbl_potion_delay_PD33]  DEFAULT ((0)) FOR [PD33]
GO
ALTER TABLE [dbo].[tbl_potion_delay] ADD  CONSTRAINT [DF_tbl_potion_delay_PD34]  DEFAULT ((0)) FOR [PD34]
GO
ALTER TABLE [dbo].[tbl_potion_delay] ADD  CONSTRAINT [DF_tbl_potion_delay_PD35]  DEFAULT ((0)) FOR [PD35]
GO
ALTER TABLE [dbo].[tbl_potion_delay] ADD  CONSTRAINT [DF__tbl_potion_delay_PD36]  DEFAULT ((0)) FOR [PD36]
GO
ALTER TABLE [dbo].[tbl_potion_delay] ADD  CONSTRAINT [DF__tbl_potion_delay_PD37]  DEFAULT ((0)) FOR [PD37]
GO
ALTER TABLE [dbo].[tbl_PrimiumPlayTime] ADD  CONSTRAINT [DF_tbl_PrimiumPlayTime_LastConnTime]  DEFAULT ((0)) FOR [LastConnTime]
GO
ALTER TABLE [dbo].[tbl_PrimiumPlayTime] ADD  CONSTRAINT [DF_tbl_PrimiumPlayTime_ContPlayTime]  DEFAULT ((0)) FOR [ContPlayTime]
GO
ALTER TABLE [dbo].[tbl_PrimiumPlayTime] ADD  CONSTRAINT [DF_tbl_PrimiumPlayTime_ForcedClose]  DEFAULT ((0)) FOR [ForcedClose]
GO
ALTER TABLE [dbo].[tbl_PrimiumPlayTime] ADD  CONSTRAINT [DF_tbl_PrimiumPlayTime_ReceiveCoupon]  DEFAULT ((0)) FOR [ReceiveCoupon]
GO
ALTER TABLE [dbo].[tbl_PrimiumPlayTime] ADD  CONSTRAINT [DF_tbl_PrimiumPlayTime_EnsureTime]  DEFAULT ((0)) FOR [EnsureTime]
GO
ALTER TABLE [dbo].[tbl_Punishment] ADD  CONSTRAINT [DF__tbl_Punis__LTime__312E5038]  DEFAULT ((0)) FOR [LTime]
GO
ALTER TABLE [dbo].[tbl_pvporderview] ADD  CONSTRAINT [DF_tbl_pvporderview_UpdateDate]  DEFAULT ((0)) FOR [UpdateDate]
GO
ALTER TABLE [dbo].[tbl_pvporderview] ADD  CONSTRAINT [DF_tbl_pvporderview_Death]  DEFAULT ((0)) FOR [Death]
GO
ALTER TABLE [dbo].[tbl_pvporderview] ADD  CONSTRAINT [DF_tbl_pvporderview_Kill]  DEFAULT ((0)) FOR [Kill]
GO
ALTER TABLE [dbo].[tbl_pvporderview] ADD  CONSTRAINT [DF_tbl_pvporderview_TodayStacked]  DEFAULT ((0)) FOR [TodayStacked]
GO
ALTER TABLE [dbo].[tbl_pvporderview] ADD  CONSTRAINT [DF_tbl_pvporderview_PvpPoint]  DEFAULT ((0)) FOR [PvpPoint]
GO
ALTER TABLE [dbo].[tbl_pvporderview] ADD  CONSTRAINT [DF_tbl_pvporderview_PvpTempCash]  DEFAULT ((0)) FOR [PvpTempCash]
GO
ALTER TABLE [dbo].[tbl_pvporderview] ADD  CONSTRAINT [DF_tbl_pvporderview_PvpCash]  DEFAULT ((0)) FOR [PvpCash]
GO
ALTER TABLE [dbo].[tbl_pvporderview] ADD  CONSTRAINT [DF_tbl_pvporderview_KillerSerial0]  DEFAULT ((0)) FOR [KillerSerial0]
GO
ALTER TABLE [dbo].[tbl_pvporderview] ADD  CONSTRAINT [DF_tbl_pvporderview_KillerSerial1]  DEFAULT ((0)) FOR [KillerSerial1]
GO
ALTER TABLE [dbo].[tbl_pvporderview] ADD  CONSTRAINT [DF_tbl_pvporderview_KillerSerial2]  DEFAULT ((0)) FOR [KillerSerial2]
GO
ALTER TABLE [dbo].[tbl_pvporderview] ADD  CONSTRAINT [DF_tbl_pvporderview_KillerSerial3]  DEFAULT ((0)) FOR [KillerSerial3]
GO
ALTER TABLE [dbo].[tbl_pvporderview] ADD  CONSTRAINT [DF_tbl_pvporderview_KillerSerial4]  DEFAULT ((0)) FOR [KillerSerial4]
GO
ALTER TABLE [dbo].[tbl_pvporderview] ADD  CONSTRAINT [DF_tbl_pvporderview_KillerSerial5]  DEFAULT ((0)) FOR [KillerSerial5]
GO
ALTER TABLE [dbo].[tbl_pvporderview] ADD  CONSTRAINT [DF_tbl_pvporderview_KillerSerial6]  DEFAULT ((0)) FOR [KillerSerial6]
GO
ALTER TABLE [dbo].[tbl_pvporderview] ADD  CONSTRAINT [DF_tbl_pvporderview_KillerSerial7]  DEFAULT ((0)) FOR [KillerSerial7]
GO
ALTER TABLE [dbo].[tbl_pvporderview] ADD  CONSTRAINT [DF_tbl_pvporderview_KillerSerial8]  DEFAULT ((0)) FOR [KillerSerial8]
GO
ALTER TABLE [dbo].[tbl_pvporderview] ADD  CONSTRAINT [DF_tbl_pvporderview_KillerSerial9]  DEFAULT ((0)) FOR [KillerSerial9]
GO
ALTER TABLE [dbo].[tbl_pvporderview] ADD  CONSTRAINT [DF_tbl_pvporderview_ContHaveCash]  DEFAULT ((0)) FOR [ContHaveCash]
GO
ALTER TABLE [dbo].[tbl_pvporderview] ADD  CONSTRAINT [DF_tbl_pvporderview_ContLoseCash]  DEFAULT ((0)) FOR [ContLoseCash]
GO
ALTER TABLE [dbo].[tbl_pvporderview] ADD  CONSTRAINT [DF_tbl_pvporderview_RaceWarRecvr]  DEFAULT ((0)) FOR [RaceWarRecvr]
GO
ALTER TABLE [dbo].[tbl_pvppointlimitinfo] ADD  CONSTRAINT [DF_tbl_pvppointlimitinfo_dck]  DEFAULT ((0)) FOR [dck]
GO
ALTER TABLE [dbo].[tbl_pvppointlimitinfo] ADD  CONSTRAINT [DF_tbl_pvppointlimitinfo_useup]  DEFAULT ((0)) FOR [useup]
GO
ALTER TABLE [dbo].[tbl_pvppointlimitinfo] ADD  CONSTRAINT [DF_tbl_pvppointlimitinfo_updatedate]  DEFAULT ((0)) FOR [updatedate]
GO
ALTER TABLE [dbo].[tbl_pvppointlimitinfo] ADD  CONSTRAINT [DF_tbl_pvppointlimitinfo_limitrate]  DEFAULT ((0)) FOR [limitrate]
GO
ALTER TABLE [dbo].[tbl_pvppointlimitinfo] ADD  CONSTRAINT [DF_tbl_pvppointlimitinfo_originalpoint]  DEFAULT ((0)) FOR [originalpoint]
GO
ALTER TABLE [dbo].[tbl_pvppointlimitinfo] ADD  CONSTRAINT [DF_tbl_pvppointlimitinfo_limitpoint]  DEFAULT ((0)) FOR [limitpoint]
GO
ALTER TABLE [dbo].[tbl_pvppointlimitinfo] ADD  CONSTRAINT [DF_tbl_pvppointlimitinfo_usepoint]  DEFAULT ((0)) FOR [usepoint]
GO
ALTER TABLE [dbo].[tbl_quest] ADD  CONSTRAINT [DF_tbl_quest_Type0]  DEFAULT (0xFF) FOR [Type0]
GO
ALTER TABLE [dbo].[tbl_quest] ADD  CONSTRAINT [DF_tbl_quest_QuestIndex0]  DEFAULT (0xFFFF) FOR [QuestIndex0]
GO
ALTER TABLE [dbo].[tbl_quest] ADD  CONSTRAINT [DF_tbl_quest_FirstAct0]  DEFAULT (0xFFFF) FOR [FirstAct0]
GO
ALTER TABLE [dbo].[tbl_quest] ADD  CONSTRAINT [DF_tbl_quest_SecondAct0]  DEFAULT (0xFFFF) FOR [SecondAct0]
GO
ALTER TABLE [dbo].[tbl_quest] ADD  CONSTRAINT [DF_tbl_quest_ThirdAct0]  DEFAULT (0xFFFF) FOR [ThirdAct0]
GO
ALTER TABLE [dbo].[tbl_quest] ADD  CONSTRAINT [DF_tbl_quest_LeftMin0]  DEFAULT (0xFFFF) FOR [PassSec0]
GO
ALTER TABLE [dbo].[tbl_quest] ADD  CONSTRAINT [DF_tbl_quest_Type1]  DEFAULT (0xFF) FOR [Type1]
GO
ALTER TABLE [dbo].[tbl_quest] ADD  CONSTRAINT [DF_tbl_quest_QuestIndex1]  DEFAULT (0xFFFF) FOR [QuestIndex1]
GO
ALTER TABLE [dbo].[tbl_quest] ADD  CONSTRAINT [DF_tbl_quest_FirstAct1]  DEFAULT (0xFFFF) FOR [FirstAct1]
GO
ALTER TABLE [dbo].[tbl_quest] ADD  CONSTRAINT [DF_tbl_quest_SecondAct1]  DEFAULT (0xFFFF) FOR [SecondAct1]
GO
ALTER TABLE [dbo].[tbl_quest] ADD  CONSTRAINT [DF_tbl_quest_ThirdAct1]  DEFAULT (0xFFFF) FOR [ThirdAct1]
GO
ALTER TABLE [dbo].[tbl_quest] ADD  CONSTRAINT [DF_tbl_quest_LeftMin1]  DEFAULT (0xFFFF) FOR [PassSec1]
GO
ALTER TABLE [dbo].[tbl_quest] ADD  CONSTRAINT [DF_tbl_quest_Type2]  DEFAULT (0xFF) FOR [Type2]
GO
ALTER TABLE [dbo].[tbl_quest] ADD  CONSTRAINT [DF_tbl_quest_QuestIndex2]  DEFAULT (0xFFFF) FOR [QuestIndex2]
GO
ALTER TABLE [dbo].[tbl_quest] ADD  CONSTRAINT [DF_tbl_quest_FirstAct2]  DEFAULT (0xFFFF) FOR [FirstAct2]
GO
ALTER TABLE [dbo].[tbl_quest] ADD  CONSTRAINT [DF_tbl_quest_SecondAct2]  DEFAULT (0xFFFF) FOR [SecondAct2]
GO
ALTER TABLE [dbo].[tbl_quest] ADD  CONSTRAINT [DF_tbl_quest_ThirdAct2]  DEFAULT (0xFFFF) FOR [ThirdAct2]
GO
ALTER TABLE [dbo].[tbl_quest] ADD  CONSTRAINT [DF_tbl_quest_LeftMin2]  DEFAULT (0xFFFF) FOR [PassSec2]
GO
ALTER TABLE [dbo].[tbl_quest] ADD  CONSTRAINT [DF_tbl_quest_Type3]  DEFAULT (0xFF) FOR [Type3]
GO
ALTER TABLE [dbo].[tbl_quest] ADD  CONSTRAINT [DF_tbl_quest_QuestIndex3]  DEFAULT (0xFFFF) FOR [QuestIndex3]
GO
ALTER TABLE [dbo].[tbl_quest] ADD  CONSTRAINT [DF_tbl_quest_FirstAct3]  DEFAULT (0xFFFF) FOR [FirstAct3]
GO
ALTER TABLE [dbo].[tbl_quest] ADD  CONSTRAINT [DF_tbl_quest_SecondAct3]  DEFAULT (0xFFFF) FOR [SecondAct3]
GO
ALTER TABLE [dbo].[tbl_quest] ADD  CONSTRAINT [DF_tbl_quest_ThirdAct3]  DEFAULT (0xFFFF) FOR [ThirdAct3]
GO
ALTER TABLE [dbo].[tbl_quest] ADD  CONSTRAINT [DF_tbl_quest_LeftMin3]  DEFAULT (0xFFFF) FOR [PassSec3]
GO
ALTER TABLE [dbo].[tbl_quest] ADD  CONSTRAINT [DF_tbl_quest_Type4]  DEFAULT (0xFF) FOR [Type4]
GO
ALTER TABLE [dbo].[tbl_quest] ADD  CONSTRAINT [DF_tbl_quest_QuestIndex4]  DEFAULT (0xFFFF) FOR [QuestIndex4]
GO
ALTER TABLE [dbo].[tbl_quest] ADD  CONSTRAINT [DF_tbl_quest_FirstAct4]  DEFAULT (0xFFFF) FOR [FirstAct4]
GO
ALTER TABLE [dbo].[tbl_quest] ADD  CONSTRAINT [DF_tbl_quest_SecondAct4]  DEFAULT (0xFFFF) FOR [SecondAct4]
GO
ALTER TABLE [dbo].[tbl_quest] ADD  CONSTRAINT [DF_tbl_quest_ThirdAct4]  DEFAULT (0xFFFF) FOR [ThirdAct4]
GO
ALTER TABLE [dbo].[tbl_quest] ADD  CONSTRAINT [DF_tbl_quest_LeftMin4]  DEFAULT (0xFFFF) FOR [PassSec4]
GO
ALTER TABLE [dbo].[tbl_quest] ADD  CONSTRAINT [DF_tbl_quest_Type5]  DEFAULT (0xFF) FOR [Type5]
GO
ALTER TABLE [dbo].[tbl_quest] ADD  CONSTRAINT [DF_tbl_quest_QuestIndex5]  DEFAULT (0xFFFF) FOR [QuestIndex5]
GO
ALTER TABLE [dbo].[tbl_quest] ADD  CONSTRAINT [DF_tbl_quest_FirstAct5]  DEFAULT (0xFFFF) FOR [FirstAct5]
GO
ALTER TABLE [dbo].[tbl_quest] ADD  CONSTRAINT [DF_tbl_quest_SecondAct5]  DEFAULT (0xFFFF) FOR [SecondAct5]
GO
ALTER TABLE [dbo].[tbl_quest] ADD  CONSTRAINT [DF_tbl_quest_ThirdAct5]  DEFAULT (0xFFFF) FOR [ThirdAct5]
GO
ALTER TABLE [dbo].[tbl_quest] ADD  CONSTRAINT [DF_tbl_quest_LeftMin5]  DEFAULT (0xFFFF) FOR [PassSec5]
GO
ALTER TABLE [dbo].[tbl_quest] ADD  CONSTRAINT [DF_tbl_quest_Type6]  DEFAULT (0xFF) FOR [Type6]
GO
ALTER TABLE [dbo].[tbl_quest] ADD  CONSTRAINT [DF_tbl_quest_QuestIndex6]  DEFAULT (0xFFFF) FOR [QuestIndex6]
GO
ALTER TABLE [dbo].[tbl_quest] ADD  CONSTRAINT [DF_tbl_quest_FirstAct6]  DEFAULT (0xFFFF) FOR [FirstAct6]
GO
ALTER TABLE [dbo].[tbl_quest] ADD  CONSTRAINT [DF_tbl_quest_SecondAct6]  DEFAULT (0xFFFF) FOR [SecondAct6]
GO
ALTER TABLE [dbo].[tbl_quest] ADD  CONSTRAINT [DF_tbl_quest_ThirdAct6]  DEFAULT (0xFFFF) FOR [ThirdAct6]
GO
ALTER TABLE [dbo].[tbl_quest] ADD  CONSTRAINT [DF_tbl_quest_LeftMin6]  DEFAULT (0xFFFF) FOR [PassSec6]
GO
ALTER TABLE [dbo].[tbl_quest] ADD  CONSTRAINT [DF_tbl_quest_Type7]  DEFAULT (0xFF) FOR [Type7]
GO
ALTER TABLE [dbo].[tbl_quest] ADD  CONSTRAINT [DF_tbl_quest_QuestIndex7]  DEFAULT (0xFFFF) FOR [QuestIndex7]
GO
ALTER TABLE [dbo].[tbl_quest] ADD  CONSTRAINT [DF_tbl_quest_FirstAct7]  DEFAULT (0xFFFF) FOR [FirstAct7]
GO
ALTER TABLE [dbo].[tbl_quest] ADD  CONSTRAINT [DF_tbl_quest_SecondAct7]  DEFAULT (0xFFFF) FOR [SecondAct7]
GO
ALTER TABLE [dbo].[tbl_quest] ADD  CONSTRAINT [DF_tbl_quest_ThirdAct7]  DEFAULT (0xFFFF) FOR [ThirdAct7]
GO
ALTER TABLE [dbo].[tbl_quest] ADD  CONSTRAINT [DF_tbl_quest_LeftMin7]  DEFAULT (0xFFFF) FOR [PassSec7]
GO
ALTER TABLE [dbo].[tbl_quest] ADD  CONSTRAINT [DF_tbl_quest_Type8]  DEFAULT (0xFF) FOR [Type8]
GO
ALTER TABLE [dbo].[tbl_quest] ADD  CONSTRAINT [DF_tbl_quest_QuestIndex8]  DEFAULT (0xFFFF) FOR [QuestIndex8]
GO
ALTER TABLE [dbo].[tbl_quest] ADD  CONSTRAINT [DF_tbl_quest_FirstAct8]  DEFAULT (0xFFFF) FOR [FirstAct8]
GO
ALTER TABLE [dbo].[tbl_quest] ADD  CONSTRAINT [DF_tbl_quest_SecondAct8]  DEFAULT (0xFFFF) FOR [SecondAct8]
GO
ALTER TABLE [dbo].[tbl_quest] ADD  CONSTRAINT [DF_tbl_quest_ThirdAct8]  DEFAULT (0xFFFF) FOR [ThirdAct8]
GO
ALTER TABLE [dbo].[tbl_quest] ADD  CONSTRAINT [DF_tbl_quest_LeftMin8]  DEFAULT (0xFFFF) FOR [PassSec8]
GO
ALTER TABLE [dbo].[tbl_quest] ADD  CONSTRAINT [DF_tbl_quest_Type9]  DEFAULT (0xFF) FOR [Type9]
GO
ALTER TABLE [dbo].[tbl_quest] ADD  CONSTRAINT [DF_tbl_quest_QuestIndex9]  DEFAULT (0xFFFF) FOR [QuestIndex9]
GO
ALTER TABLE [dbo].[tbl_quest] ADD  CONSTRAINT [DF_tbl_quest_FirstAct9]  DEFAULT (0xFFFF) FOR [FirstAct9]
GO
ALTER TABLE [dbo].[tbl_quest] ADD  CONSTRAINT [DF_tbl_quest_SecondAct9]  DEFAULT (0xFFFF) FOR [SecondAct9]
GO
ALTER TABLE [dbo].[tbl_quest] ADD  CONSTRAINT [DF_tbl_quest_ThirdAct9]  DEFAULT (0xFFFF) FOR [ThirdAct9]
GO
ALTER TABLE [dbo].[tbl_quest] ADD  CONSTRAINT [DF_tbl_quest_LeftMin9]  DEFAULT (0xFFFF) FOR [PassSec9]
GO
ALTER TABLE [dbo].[tbl_quest] ADD  CONSTRAINT [DF_tbl_quest_Type10]  DEFAULT (0xFF) FOR [Type10]
GO
ALTER TABLE [dbo].[tbl_quest] ADD  CONSTRAINT [DF_tbl_quest_QuestIndex10]  DEFAULT (0xFFFF) FOR [QuestIndex10]
GO
ALTER TABLE [dbo].[tbl_quest] ADD  CONSTRAINT [DF_tbl_quest_FirstAct10]  DEFAULT (0xFFFF) FOR [FirstAct10]
GO
ALTER TABLE [dbo].[tbl_quest] ADD  CONSTRAINT [DF_tbl_quest_SecondAct10]  DEFAULT (0xFFFF) FOR [SecondAct10]
GO
ALTER TABLE [dbo].[tbl_quest] ADD  CONSTRAINT [DF_tbl_quest_ThirdAct10]  DEFAULT (0xFFFF) FOR [ThirdAct10]
GO
ALTER TABLE [dbo].[tbl_quest] ADD  CONSTRAINT [DF_tbl_quest_LeftMin10]  DEFAULT (0xFFFF) FOR [PassSec10]
GO
ALTER TABLE [dbo].[tbl_quest] ADD  CONSTRAINT [DF_tbl_quest_Type11]  DEFAULT (0xFF) FOR [Type11]
GO
ALTER TABLE [dbo].[tbl_quest] ADD  CONSTRAINT [DF_tbl_quest_QuestIndex11]  DEFAULT (0xFFFF) FOR [QuestIndex11]
GO
ALTER TABLE [dbo].[tbl_quest] ADD  CONSTRAINT [DF_tbl_quest_FirstAct11]  DEFAULT (0xFFFF) FOR [FirstAct11]
GO
ALTER TABLE [dbo].[tbl_quest] ADD  CONSTRAINT [DF_tbl_quest_SecondAct11]  DEFAULT (0xFFFF) FOR [SecondAct11]
GO
ALTER TABLE [dbo].[tbl_quest] ADD  CONSTRAINT [DF_tbl_quest_ThirdAct11]  DEFAULT (0xFFFF) FOR [ThirdAct11]
GO
ALTER TABLE [dbo].[tbl_quest] ADD  CONSTRAINT [DF_tbl_quest_LeftMin11]  DEFAULT (0xFFFF) FOR [PassSec11]
GO
ALTER TABLE [dbo].[tbl_quest] ADD  CONSTRAINT [DF_tbl_quest_Type12]  DEFAULT (0xFF) FOR [Type12]
GO
ALTER TABLE [dbo].[tbl_quest] ADD  CONSTRAINT [DF_tbl_quest_QuestIndex12]  DEFAULT (0xFFFF) FOR [QuestIndex12]
GO
ALTER TABLE [dbo].[tbl_quest] ADD  CONSTRAINT [DF_tbl_quest_FirstAct12]  DEFAULT (0xFFFF) FOR [FirstAct12]
GO
ALTER TABLE [dbo].[tbl_quest] ADD  CONSTRAINT [DF_tbl_quest_SecondAct12]  DEFAULT (0xFFFF) FOR [SecondAct12]
GO
ALTER TABLE [dbo].[tbl_quest] ADD  CONSTRAINT [DF_tbl_quest_ThirdAct12]  DEFAULT (0xFFFF) FOR [ThirdAct12]
GO
ALTER TABLE [dbo].[tbl_quest] ADD  CONSTRAINT [DF_tbl_quest_LeftMin12]  DEFAULT (0xFFFF) FOR [PassSec12]
GO
ALTER TABLE [dbo].[tbl_quest] ADD  CONSTRAINT [DF_tbl_quest_Type13]  DEFAULT (0xFF) FOR [Type13]
GO
ALTER TABLE [dbo].[tbl_quest] ADD  CONSTRAINT [DF_tbl_quest_QuestIndex13]  DEFAULT (0xFFFF) FOR [QuestIndex13]
GO
ALTER TABLE [dbo].[tbl_quest] ADD  CONSTRAINT [DF_tbl_quest_FirstAct13]  DEFAULT (0xFFFF) FOR [FirstAct13]
GO
ALTER TABLE [dbo].[tbl_quest] ADD  CONSTRAINT [DF_tbl_quest_SecondAct13]  DEFAULT (0xFFFF) FOR [SecondAct13]
GO
ALTER TABLE [dbo].[tbl_quest] ADD  CONSTRAINT [DF_tbl_quest_ThirdAct13]  DEFAULT (0xFFFF) FOR [ThirdAct13]
GO
ALTER TABLE [dbo].[tbl_quest] ADD  CONSTRAINT [DF_tbl_quest_LeftMin13]  DEFAULT (0xFFFF) FOR [PassSec13]
GO
ALTER TABLE [dbo].[tbl_quest] ADD  CONSTRAINT [DF_tbl_quest_Type14]  DEFAULT (0xFF) FOR [Type14]
GO
ALTER TABLE [dbo].[tbl_quest] ADD  CONSTRAINT [DF_tbl_quest_QuestIndex14]  DEFAULT (0xFFFF) FOR [QuestIndex14]
GO
ALTER TABLE [dbo].[tbl_quest] ADD  CONSTRAINT [DF_tbl_quest_FirstAct14]  DEFAULT (0xFFFF) FOR [FirstAct14]
GO
ALTER TABLE [dbo].[tbl_quest] ADD  CONSTRAINT [DF_tbl_quest_SecondAct14]  DEFAULT (0xFFFF) FOR [SecondAct14]
GO
ALTER TABLE [dbo].[tbl_quest] ADD  CONSTRAINT [DF_tbl_quest_ThirdAct14]  DEFAULT (0xFFFF) FOR [ThirdAct14]
GO
ALTER TABLE [dbo].[tbl_quest] ADD  CONSTRAINT [DF_tbl_quest_LeftMin14]  DEFAULT (0xFFFF) FOR [PassSec14]
GO
ALTER TABLE [dbo].[tbl_quest] ADD  CONSTRAINT [DF_tbl_quest_DCK]  DEFAULT ((0)) FOR [DCK]
GO
ALTER TABLE [dbo].[tbl_quest] ADD  CONSTRAINT [DF_tbl_quest_Type15]  DEFAULT (0xFF) FOR [Type15]
GO
ALTER TABLE [dbo].[tbl_quest] ADD  CONSTRAINT [DF_tbl_quest_QuestIndex15]  DEFAULT (0xFFFF) FOR [QuestIndex15]
GO
ALTER TABLE [dbo].[tbl_quest] ADD  CONSTRAINT [DF_tbl_quest_FirstAct15]  DEFAULT (0xFFFF) FOR [FirstAct15]
GO
ALTER TABLE [dbo].[tbl_quest] ADD  CONSTRAINT [DF_tbl_quest_SecondAct15]  DEFAULT (0xFFFF) FOR [SecondAct15]
GO
ALTER TABLE [dbo].[tbl_quest] ADD  CONSTRAINT [DF_tbl_quest_ThirdAct15]  DEFAULT (0xFFFF) FOR [ThirdAct15]
GO
ALTER TABLE [dbo].[tbl_quest] ADD  CONSTRAINT [DF_tbl_quest_PassSec15]  DEFAULT (0xFFFF) FOR [PassSec15]
GO
ALTER TABLE [dbo].[tbl_quest] ADD  CONSTRAINT [DF_tbl_quest_Type16]  DEFAULT (0xFF) FOR [Type16]
GO
ALTER TABLE [dbo].[tbl_quest] ADD  CONSTRAINT [DF_tbl_quest_QuestIndex16]  DEFAULT (0xFFFF) FOR [QuestIndex16]
GO
ALTER TABLE [dbo].[tbl_quest] ADD  CONSTRAINT [DF_tbl_quest_FirstAct16]  DEFAULT (0xFFFF) FOR [FirstAct16]
GO
ALTER TABLE [dbo].[tbl_quest] ADD  CONSTRAINT [DF_tbl_quest_SecondAct16]  DEFAULT (0xFFFF) FOR [SecondAct16]
GO
ALTER TABLE [dbo].[tbl_quest] ADD  CONSTRAINT [DF_tbl_quest_ThirdAct16]  DEFAULT (0xFFFF) FOR [ThirdAct16]
GO
ALTER TABLE [dbo].[tbl_quest] ADD  CONSTRAINT [DF_tbl_quest_PassSec16]  DEFAULT (0xFFFF) FOR [PassSec16]
GO
ALTER TABLE [dbo].[tbl_quest] ADD  CONSTRAINT [DF_tbl_quest_Type17]  DEFAULT (0xFF) FOR [Type17]
GO
ALTER TABLE [dbo].[tbl_quest] ADD  CONSTRAINT [DF_tbl_quest_QuestIndex17]  DEFAULT (0xFFFF) FOR [QuestIndex17]
GO
ALTER TABLE [dbo].[tbl_quest] ADD  CONSTRAINT [DF_tbl_quest_FirstAct17]  DEFAULT (0xFFFF) FOR [FirstAct17]
GO
ALTER TABLE [dbo].[tbl_quest] ADD  CONSTRAINT [DF_tbl_quest_SecondAct17]  DEFAULT (0xFFFF) FOR [SecondAct17]
GO
ALTER TABLE [dbo].[tbl_quest] ADD  CONSTRAINT [DF_tbl_quest_ThirdAct17]  DEFAULT (0xFFFF) FOR [ThirdAct17]
GO
ALTER TABLE [dbo].[tbl_quest] ADD  CONSTRAINT [DF_tbl_quest_PassSec17]  DEFAULT (0xFFFF) FOR [PassSec17]
GO
ALTER TABLE [dbo].[tbl_quest] ADD  CONSTRAINT [DF_tbl_quest_Type18]  DEFAULT (0xFF) FOR [Type18]
GO
ALTER TABLE [dbo].[tbl_quest] ADD  CONSTRAINT [DF_tbl_quest_QuestIndex18]  DEFAULT (0xFFFF) FOR [QuestIndex18]
GO
ALTER TABLE [dbo].[tbl_quest] ADD  CONSTRAINT [DF_tbl_quest_FirstAct18]  DEFAULT (0xFFFF) FOR [FirstAct18]
GO
ALTER TABLE [dbo].[tbl_quest] ADD  CONSTRAINT [DF_tbl_quest_SecondAct18]  DEFAULT (0xFFFF) FOR [SecondAct18]
GO
ALTER TABLE [dbo].[tbl_quest] ADD  CONSTRAINT [DF_tbl_quest_ThirdAct18]  DEFAULT (0xFFFF) FOR [ThirdAct18]
GO
ALTER TABLE [dbo].[tbl_quest] ADD  CONSTRAINT [DF_tbl_quest_PassSec18]  DEFAULT (0xFFFF) FOR [PassSec18]
GO
ALTER TABLE [dbo].[tbl_quest] ADD  CONSTRAINT [DF_tbl_quest_Type19]  DEFAULT (0xFF) FOR [Type19]
GO
ALTER TABLE [dbo].[tbl_quest] ADD  CONSTRAINT [DF_tbl_quest_QuestIndex19]  DEFAULT (0xFFFF) FOR [QuestIndex19]
GO
ALTER TABLE [dbo].[tbl_quest] ADD  CONSTRAINT [DF_tbl_quest_FirstAct19]  DEFAULT (0xFFFF) FOR [FirstAct19]
GO
ALTER TABLE [dbo].[tbl_quest] ADD  CONSTRAINT [DF_tbl_quest_SecondAct19]  DEFAULT (0xFFFF) FOR [SecondAct19]
GO
ALTER TABLE [dbo].[tbl_quest] ADD  CONSTRAINT [DF_tbl_quest_ThirdAct19]  DEFAULT (0xFFFF) FOR [ThirdAct19]
GO
ALTER TABLE [dbo].[tbl_quest] ADD  CONSTRAINT [DF_tbl_quest_PassSec19]  DEFAULT (0xFFFF) FOR [PassSec19]
GO
ALTER TABLE [dbo].[tbl_quest] ADD  CONSTRAINT [DF_tbl_quest_Type20]  DEFAULT (0xFF) FOR [Type20]
GO
ALTER TABLE [dbo].[tbl_quest] ADD  CONSTRAINT [DF_tbl_quest_QuestIndex20]  DEFAULT (0xFFFF) FOR [QuestIndex20]
GO
ALTER TABLE [dbo].[tbl_quest] ADD  CONSTRAINT [DF_tbl_quest_FirstAct20]  DEFAULT (0xFFFF) FOR [FirstAct20]
GO
ALTER TABLE [dbo].[tbl_quest] ADD  CONSTRAINT [DF_tbl_quest_SecondAct20]  DEFAULT (0xFFFF) FOR [SecondAct20]
GO
ALTER TABLE [dbo].[tbl_quest] ADD  CONSTRAINT [DF_tbl_quest_ThirdAct20]  DEFAULT (0xFFFF) FOR [ThirdAct20]
GO
ALTER TABLE [dbo].[tbl_quest] ADD  CONSTRAINT [DF_tbl_quest_PassSec20]  DEFAULT (0xFFFF) FOR [PassSec20]
GO
ALTER TABLE [dbo].[tbl_quest] ADD  CONSTRAINT [DF_tbl_quest_Type21]  DEFAULT (0xFF) FOR [Type21]
GO
ALTER TABLE [dbo].[tbl_quest] ADD  CONSTRAINT [DF_tbl_quest_QuestIndex21]  DEFAULT (0xFFFF) FOR [QuestIndex21]
GO
ALTER TABLE [dbo].[tbl_quest] ADD  CONSTRAINT [DF_tbl_quest_FirstAct21]  DEFAULT (0xFFFF) FOR [FirstAct21]
GO
ALTER TABLE [dbo].[tbl_quest] ADD  CONSTRAINT [DF_tbl_quest_SecondAct21]  DEFAULT (0xFFFF) FOR [SecondAct21]
GO
ALTER TABLE [dbo].[tbl_quest] ADD  CONSTRAINT [DF_tbl_quest_ThirdAct21]  DEFAULT (0xFFFF) FOR [ThirdAct21]
GO
ALTER TABLE [dbo].[tbl_quest] ADD  CONSTRAINT [DF_tbl_quest_PassSec21]  DEFAULT (0xFFFF) FOR [PassSec21]
GO
ALTER TABLE [dbo].[tbl_quest] ADD  CONSTRAINT [DF_tbl_quest_Type22]  DEFAULT (0xFF) FOR [Type22]
GO
ALTER TABLE [dbo].[tbl_quest] ADD  CONSTRAINT [DF_tbl_quest_QuestIndex22]  DEFAULT (0xFFFF) FOR [QuestIndex22]
GO
ALTER TABLE [dbo].[tbl_quest] ADD  CONSTRAINT [DF_tbl_quest_FirstAct22]  DEFAULT (0xFFFF) FOR [FirstAct22]
GO
ALTER TABLE [dbo].[tbl_quest] ADD  CONSTRAINT [DF_tbl_quest_SecondAct22]  DEFAULT (0xFFFF) FOR [SecondAct22]
GO
ALTER TABLE [dbo].[tbl_quest] ADD  CONSTRAINT [DF_tbl_quest_ThirdAct22]  DEFAULT (0xFFFF) FOR [ThirdAct22]
GO
ALTER TABLE [dbo].[tbl_quest] ADD  CONSTRAINT [DF_tbl_quest_PassSec22]  DEFAULT (0xFFFF) FOR [PassSec22]
GO
ALTER TABLE [dbo].[tbl_quest] ADD  CONSTRAINT [DF_tbl_quest_Type23]  DEFAULT (0xFF) FOR [Type23]
GO
ALTER TABLE [dbo].[tbl_quest] ADD  CONSTRAINT [DF_tbl_quest_QuestIndex23]  DEFAULT (0xFFFF) FOR [QuestIndex23]
GO
ALTER TABLE [dbo].[tbl_quest] ADD  CONSTRAINT [DF_tbl_quest_FirstAct23]  DEFAULT (0xFFFF) FOR [FirstAct23]
GO
ALTER TABLE [dbo].[tbl_quest] ADD  CONSTRAINT [DF_tbl_quest_SecondAct23]  DEFAULT (0xFFFF) FOR [SecondAct23]
GO
ALTER TABLE [dbo].[tbl_quest] ADD  CONSTRAINT [DF_tbl_quest_ThirdAct23]  DEFAULT (0xFFFF) FOR [ThirdAct23]
GO
ALTER TABLE [dbo].[tbl_quest] ADD  CONSTRAINT [DF_tbl_quest_PassSec23]  DEFAULT (0xFFFF) FOR [PassSec23]
GO
ALTER TABLE [dbo].[tbl_quest] ADD  CONSTRAINT [DF_tbl_quest_Type24]  DEFAULT (0xFF) FOR [Type24]
GO
ALTER TABLE [dbo].[tbl_quest] ADD  CONSTRAINT [DF_tbl_quest_QuestIndex24]  DEFAULT (0xFFFF) FOR [QuestIndex24]
GO
ALTER TABLE [dbo].[tbl_quest] ADD  CONSTRAINT [DF_tbl_quest_FirstAct24]  DEFAULT (0xFFFF) FOR [FirstAct24]
GO
ALTER TABLE [dbo].[tbl_quest] ADD  CONSTRAINT [DF_tbl_quest_SecondAct24]  DEFAULT (0xFFFF) FOR [SecondAct24]
GO
ALTER TABLE [dbo].[tbl_quest] ADD  CONSTRAINT [DF_tbl_quest_ThirdAct24]  DEFAULT (0xFFFF) FOR [ThirdAct24]
GO
ALTER TABLE [dbo].[tbl_quest] ADD  CONSTRAINT [DF_tbl_quest_PassSec24]  DEFAULT (0xFFFF) FOR [PassSec24]
GO
ALTER TABLE [dbo].[tbl_quest] ADD  CONSTRAINT [DF_tbl_quest_Type25]  DEFAULT (0xFF) FOR [Type25]
GO
ALTER TABLE [dbo].[tbl_quest] ADD  CONSTRAINT [DF_tbl_quest_QuestIndex25]  DEFAULT (0xFFFF) FOR [QuestIndex25]
GO
ALTER TABLE [dbo].[tbl_quest] ADD  CONSTRAINT [DF_tbl_quest_FirstAct25]  DEFAULT (0xFFFF) FOR [FirstAct25]
GO
ALTER TABLE [dbo].[tbl_quest] ADD  CONSTRAINT [DF_tbl_quest_SecondAct25]  DEFAULT (0xFFFF) FOR [SecondAct25]
GO
ALTER TABLE [dbo].[tbl_quest] ADD  CONSTRAINT [DF_tbl_quest_ThirdAct25]  DEFAULT (0xFFFF) FOR [ThirdAct25]
GO
ALTER TABLE [dbo].[tbl_quest] ADD  CONSTRAINT [DF_tbl_quest_PassSec25]  DEFAULT (0xFFFF) FOR [PassSec25]
GO
ALTER TABLE [dbo].[tbl_quest] ADD  CONSTRAINT [DF_tbl_quest_Type26]  DEFAULT (0xFF) FOR [Type26]
GO
ALTER TABLE [dbo].[tbl_quest] ADD  CONSTRAINT [DF_tbl_quest_QuestIndex26]  DEFAULT (0xFFFF) FOR [QuestIndex26]
GO
ALTER TABLE [dbo].[tbl_quest] ADD  CONSTRAINT [DF_tbl_quest_FirstAct26]  DEFAULT (0xFFFF) FOR [FirstAct26]
GO
ALTER TABLE [dbo].[tbl_quest] ADD  CONSTRAINT [DF_tbl_quest_SecondAct26]  DEFAULT (0xFFFF) FOR [SecondAct26]
GO
ALTER TABLE [dbo].[tbl_quest] ADD  CONSTRAINT [DF_tbl_quest_ThirdAct26]  DEFAULT (0xFFFF) FOR [ThirdAct26]
GO
ALTER TABLE [dbo].[tbl_quest] ADD  CONSTRAINT [DF_tbl_quest_PassSec26]  DEFAULT (0xFFFF) FOR [PassSec26]
GO
ALTER TABLE [dbo].[tbl_quest] ADD  CONSTRAINT [DF_tbl_quest_Type27]  DEFAULT (0xFF) FOR [Type27]
GO
ALTER TABLE [dbo].[tbl_quest] ADD  CONSTRAINT [DF_tbl_quest_QuestIndex27]  DEFAULT (0xFFFF) FOR [QuestIndex27]
GO
ALTER TABLE [dbo].[tbl_quest] ADD  CONSTRAINT [DF_tbl_quest_FirstAct27]  DEFAULT (0xFFFF) FOR [FirstAct27]
GO
ALTER TABLE [dbo].[tbl_quest] ADD  CONSTRAINT [DF_tbl_quest_SecondAct27]  DEFAULT (0xFFFF) FOR [SecondAct27]
GO
ALTER TABLE [dbo].[tbl_quest] ADD  CONSTRAINT [DF_tbl_quest_ThirdAct27]  DEFAULT (0xFFFF) FOR [ThirdAct27]
GO
ALTER TABLE [dbo].[tbl_quest] ADD  CONSTRAINT [DF_tbl_quest_PassSec27]  DEFAULT (0xFFFF) FOR [PassSec27]
GO
ALTER TABLE [dbo].[tbl_quest] ADD  CONSTRAINT [DF_tbl_quest_Type28]  DEFAULT (0xFF) FOR [Type28]
GO
ALTER TABLE [dbo].[tbl_quest] ADD  CONSTRAINT [DF_tbl_quest_QuestIndex28]  DEFAULT (0xFFFF) FOR [QuestIndex28]
GO
ALTER TABLE [dbo].[tbl_quest] ADD  CONSTRAINT [DF_tbl_quest_FirstAct28]  DEFAULT (0xFFFF) FOR [FirstAct28]
GO
ALTER TABLE [dbo].[tbl_quest] ADD  CONSTRAINT [DF_tbl_quest_SecondAct28]  DEFAULT (0xFFFF) FOR [SecondAct28]
GO
ALTER TABLE [dbo].[tbl_quest] ADD  CONSTRAINT [DF_tbl_quest_ThirdAct28]  DEFAULT (0xFFFF) FOR [ThirdAct28]
GO
ALTER TABLE [dbo].[tbl_quest] ADD  CONSTRAINT [DF_tbl_quest_PassSec28]  DEFAULT (0xFFFF) FOR [PassSec28]
GO
ALTER TABLE [dbo].[tbl_quest] ADD  CONSTRAINT [DF_tbl_quest_Type29]  DEFAULT (0xFF) FOR [Type29]
GO
ALTER TABLE [dbo].[tbl_quest] ADD  CONSTRAINT [DF_tbl_quest_QuestIndex29]  DEFAULT (0xFFFF) FOR [QuestIndex29]
GO
ALTER TABLE [dbo].[tbl_quest] ADD  CONSTRAINT [DF_tbl_quest_FirstAct29]  DEFAULT (0xFFFF) FOR [FirstAct29]
GO
ALTER TABLE [dbo].[tbl_quest] ADD  CONSTRAINT [DF_tbl_quest_SecondAct29]  DEFAULT (0xFFFF) FOR [SecondAct29]
GO
ALTER TABLE [dbo].[tbl_quest] ADD  CONSTRAINT [DF_tbl_quest_ThirdAct29]  DEFAULT (0xFFFF) FOR [ThirdAct29]
GO
ALTER TABLE [dbo].[tbl_quest] ADD  CONSTRAINT [DF_tbl_quest_PassSec29]  DEFAULT (0xFFFF) FOR [PassSec29]
GO
ALTER TABLE [dbo].[tbl_racebattle_log] ADD  CONSTRAINT [DF_tbl_racebattle_log_nth]  DEFAULT ((0)) FOR [nth]
GO
ALTER TABLE [dbo].[tbl_racebattle_log] ADD  CONSTRAINT [DF_tbl_racebattle_log_winrace]  DEFAULT (0xFF) FOR [winrace]
GO
ALTER TABLE [dbo].[tbl_racebattle_log] ADD  CONSTRAINT [DF_tbl_racebattle_log_loserace]  DEFAULT (0xFF) FOR [loserace]
GO
ALTER TABLE [dbo].[tbl_racebattle_log] ADD  CONSTRAINT [DF_tbl_racebattle_log_regdate]  DEFAULT (getdate()) FOR [regdate]
GO
ALTER TABLE [dbo].[tbl_racebattle_log] ADD  CONSTRAINT [DF__tbl_raceb__bosss__398F5766]  DEFAULT (0xFFFFFFFF) FOR [bossserial0]
GO
ALTER TABLE [dbo].[tbl_racebattle_log] ADD  CONSTRAINT [DF__tbl_raceb__bosss__3A837B9F]  DEFAULT (0xFFFFFFFF) FOR [bossserial1]
GO
ALTER TABLE [dbo].[tbl_racebattle_log] ADD  CONSTRAINT [DF__tbl_raceb__bosss__3B779FD8]  DEFAULT (0xFFFFFFFF) FOR [bossserial2]
GO
ALTER TABLE [dbo].[tbl_RemainOre_Log] ADD  CONSTRAINT [DF_tbl_RemainOre_Log_LogDate]  DEFAULT (getdate()) FOR [logDate]
GO
ALTER TABLE [dbo].[tbl_RenamePotion_Log] ADD  CONSTRAINT [DF_tbl_RenamePotion_Log_Serial]  DEFAULT ((0)) FOR [Serial]
GO
ALTER TABLE [dbo].[tbl_RenamePotion_Log] ADD  CONSTRAINT [DF_tbl_RenamePotion_Log_OldName]  DEFAULT ('*') FOR [OldName]
GO
ALTER TABLE [dbo].[tbl_RenamePotion_Log] ADD  CONSTRAINT [DF_tbl_RenamePotion_Log_NewName]  DEFAULT ('*') FOR [NewName]
GO
ALTER TABLE [dbo].[tbl_start_npc_quest_history] ADD  CONSTRAINT [DF_tbl_start_npc_quest_history_EndTime]  DEFAULT ((0)) FOR [EndTime]
GO
ALTER TABLE [dbo].[tbl_StoreLimitItem_061212] ADD  CONSTRAINT [DF__tbl_StoreL__type__577192AC]  DEFAULT ((0)) FOR [type]
GO
ALTER TABLE [dbo].[tbl_StoreLimitItem_061212] ADD  CONSTRAINT [DF__tbl_StoreLi__dck__5865B6E5]  DEFAULT ((1)) FOR [dck]
GO
ALTER TABLE [dbo].[tbl_StoreLimitItem_061212] ADD  CONSTRAINT [DF__tbl_StoreLim__k0__5959DB1E]  DEFAULT (0xFFFFFFFF) FOR [k0]
GO
ALTER TABLE [dbo].[tbl_StoreLimitItem_061212] ADD  CONSTRAINT [DF__tbl_StoreL__num0__5A4DFF57]  DEFAULT ((0)) FOR [num0]
GO
ALTER TABLE [dbo].[tbl_StoreLimitItem_061212] ADD  CONSTRAINT [DF__tbl_StoreLim__k1__5B422390]  DEFAULT (0xFFFFFFFF) FOR [k1]
GO
ALTER TABLE [dbo].[tbl_StoreLimitItem_061212] ADD  CONSTRAINT [DF__tbl_StoreL__num1__5C3647C9]  DEFAULT ((0)) FOR [num1]
GO
ALTER TABLE [dbo].[tbl_StoreLimitItem_061212] ADD  CONSTRAINT [DF__tbl_StoreLim__k2__5D2A6C02]  DEFAULT (0xFFFFFFFF) FOR [k2]
GO
ALTER TABLE [dbo].[tbl_StoreLimitItem_061212] ADD  CONSTRAINT [DF__tbl_StoreL__num2__5E1E903B]  DEFAULT ((0)) FOR [num2]
GO
ALTER TABLE [dbo].[tbl_StoreLimitItem_061212] ADD  CONSTRAINT [DF__tbl_StoreLim__k3__5F12B474]  DEFAULT (0xFFFFFFFF) FOR [k3]
GO
ALTER TABLE [dbo].[tbl_StoreLimitItem_061212] ADD  CONSTRAINT [DF__tbl_StoreL__num3__6006D8AD]  DEFAULT ((0)) FOR [num3]
GO
ALTER TABLE [dbo].[tbl_StoreLimitItem_061212] ADD  CONSTRAINT [DF__tbl_StoreLim__k4__60FAFCE6]  DEFAULT (0xFFFFFFFF) FOR [k4]
GO
ALTER TABLE [dbo].[tbl_StoreLimitItem_061212] ADD  CONSTRAINT [DF__tbl_StoreL__num4__61EF211F]  DEFAULT ((0)) FOR [num4]
GO
ALTER TABLE [dbo].[tbl_StoreLimitItem_061212] ADD  CONSTRAINT [DF__tbl_StoreLim__k5__62E34558]  DEFAULT (0xFFFFFFFF) FOR [k5]
GO
ALTER TABLE [dbo].[tbl_StoreLimitItem_061212] ADD  CONSTRAINT [DF__tbl_StoreL__num5__63D76991]  DEFAULT ((0)) FOR [num5]
GO
ALTER TABLE [dbo].[tbl_StoreLimitItem_061212] ADD  CONSTRAINT [DF__tbl_StoreLim__k6__64CB8DCA]  DEFAULT (0xFFFFFFFF) FOR [k6]
GO
ALTER TABLE [dbo].[tbl_StoreLimitItem_061212] ADD  CONSTRAINT [DF__tbl_StoreL__num6__65BFB203]  DEFAULT ((0)) FOR [num6]
GO
ALTER TABLE [dbo].[tbl_StoreLimitItem_061212] ADD  CONSTRAINT [DF__tbl_StoreLim__k7__66B3D63C]  DEFAULT (0xFFFFFFFF) FOR [k7]
GO
ALTER TABLE [dbo].[tbl_StoreLimitItem_061212] ADD  CONSTRAINT [DF__tbl_StoreL__num7__67A7FA75]  DEFAULT ((0)) FOR [num7]
GO
ALTER TABLE [dbo].[tbl_StoreLimitItem_061212] ADD  CONSTRAINT [DF__tbl_StoreLim__k8__689C1EAE]  DEFAULT (0xFFFFFFFF) FOR [k8]
GO
ALTER TABLE [dbo].[tbl_StoreLimitItem_061212] ADD  CONSTRAINT [DF__tbl_StoreL__num8__699042E7]  DEFAULT ((0)) FOR [num8]
GO
ALTER TABLE [dbo].[tbl_StoreLimitItem_061212] ADD  CONSTRAINT [DF__tbl_StoreLim__k9__6A846720]  DEFAULT (0xFFFFFFFF) FOR [k9]
GO
ALTER TABLE [dbo].[tbl_StoreLimitItem_061212] ADD  CONSTRAINT [DF__tbl_StoreL__num9__6B788B59]  DEFAULT ((0)) FOR [num9]
GO
ALTER TABLE [dbo].[tbl_StoreLimitItem_061212] ADD  CONSTRAINT [DF__tbl_StoreLi__k10__6C6CAF92]  DEFAULT (0xFFFFFFFF) FOR [k10]
GO
ALTER TABLE [dbo].[tbl_StoreLimitItem_061212] ADD  CONSTRAINT [DF__tbl_Store__num10__6D60D3CB]  DEFAULT ((0)) FOR [num10]
GO
ALTER TABLE [dbo].[tbl_StoreLimitItem_061212] ADD  CONSTRAINT [DF__tbl_StoreLi__k11__6E54F804]  DEFAULT (0xFFFFFFFF) FOR [k11]
GO
ALTER TABLE [dbo].[tbl_StoreLimitItem_061212] ADD  CONSTRAINT [DF__tbl_Store__num11__6F491C3D]  DEFAULT ((0)) FOR [num11]
GO
ALTER TABLE [dbo].[tbl_StoreLimitItem_061212] ADD  CONSTRAINT [DF__tbl_StoreLi__k12__703D4076]  DEFAULT (0xFFFFFFFF) FOR [k12]
GO
ALTER TABLE [dbo].[tbl_StoreLimitItem_061212] ADD  CONSTRAINT [DF__tbl_Store__num12__713164AF]  DEFAULT ((0)) FOR [num12]
GO
ALTER TABLE [dbo].[tbl_StoreLimitItem_061212] ADD  CONSTRAINT [DF__tbl_StoreLi__k13__722588E8]  DEFAULT (0xFFFFFFFF) FOR [k13]
GO
ALTER TABLE [dbo].[tbl_StoreLimitItem_061212] ADD  CONSTRAINT [DF__tbl_Store__num13__7319AD21]  DEFAULT ((0)) FOR [num13]
GO
ALTER TABLE [dbo].[tbl_StoreLimitItem_061212] ADD  CONSTRAINT [DF__tbl_StoreLi__k14__740DD15A]  DEFAULT (0xFFFFFFFF) FOR [k14]
GO
ALTER TABLE [dbo].[tbl_StoreLimitItem_061212] ADD  CONSTRAINT [DF__tbl_Store__num14__7501F593]  DEFAULT ((0)) FOR [num14]
GO
ALTER TABLE [dbo].[tbl_StoreLimitItem_061212] ADD  CONSTRAINT [DF__tbl_StoreLi__k15__75F619CC]  DEFAULT (0xFFFFFFFF) FOR [k15]
GO
ALTER TABLE [dbo].[tbl_StoreLimitItem_061212] ADD  CONSTRAINT [DF__tbl_Store__num15__76EA3E05]  DEFAULT ((0)) FOR [num15]
GO
ALTER TABLE [dbo].[tbl_StoreLimitItem_061212] ADD  CONSTRAINT [DF_tbl_StoreLimit_resettime]  DEFAULT ((0)) FOR [resettime]
GO
ALTER TABLE [dbo].[tbl_supplement] ADD  CONSTRAINT [DF__tbl_supplement_DCK]  DEFAULT ((0)) FOR [DCK]
GO
ALTER TABLE [dbo].[tbl_supplement] ADD  CONSTRAINT [DF__tbl_supplement_PvpPointleak]  DEFAULT ((0)) FOR [PvpPointLeak]
GO
ALTER TABLE [dbo].[tbl_supplement] ADD  CONSTRAINT [DF__tbl_supplement_LastAttBuff]  DEFAULT ((0)) FOR [LastAttBuff]
GO
ALTER TABLE [dbo].[tbl_supplement] ADD  CONSTRAINT [DF_tbl_supplement_cristalbattledate]  DEFAULT ((0)) FOR [cristalbattledate]
GO
ALTER TABLE [dbo].[tbl_supplement] ADD  CONSTRAINT [DF__tbl_supplement_BufEndTime]  DEFAULT ((0)) FOR [BufEndTime]
GO
ALTER TABLE [dbo].[tbl_supplement] ADD  CONSTRAINT [DF__tbl_supplement_RaceBuffClear]  DEFAULT ((0)) FOR [RaceBuffClear]
GO
ALTER TABLE [dbo].[tbl_supplement] ADD  CONSTRAINT [DF_tbl_Supplement_ScanerCnt]  DEFAULT ((1900010110)) FOR [ScanerCnt]
GO
ALTER TABLE [dbo].[tbl_supplement] ADD  CONSTRAINT [DF_tbl_Supplement_AccumPlayTime]  DEFAULT ((0)) FOR [AccumPlayTime]
GO
ALTER TABLE [dbo].[tbl_supplement] ADD  CONSTRAINT [DF_tbl_Supplement_IsVoted]  DEFAULT ((0)) FOR [IsVoted]
GO
ALTER TABLE [dbo].[tbl_supplement] ADD  CONSTRAINT [DF_tbl_base_VoteEnable]  DEFAULT ((1)) FOR [VoteEnable]
GO
ALTER TABLE [dbo].[tbl_supplement] ADD  CONSTRAINT [DF_tbl_Supplement_ResetAccumPlayTime]  DEFAULT ((1990011)) FOR [ResetAccumPlayTime]
GO
ALTER TABLE [dbo].[tbl_supplement] ADD  CONSTRAINT [DF_tbl_Supplement_ActionPoint_0]  DEFAULT ((0)) FOR [ActionPoint_0]
GO
ALTER TABLE [dbo].[tbl_supplement] ADD  CONSTRAINT [DF_tbl_Supplement_ActionPoint_1]  DEFAULT ((0)) FOR [ActionPoint_1]
GO
ALTER TABLE [dbo].[tbl_supplement] ADD  CONSTRAINT [DF_tbl_Supplement_ActionPoint_2]  DEFAULT ((0)) FOR [ActionPoint_2]
GO
ALTER TABLE [dbo].[tbl_supplement] ADD  CONSTRAINT [DF_tbl_supplement_EK6]  DEFAULT (0xFFFFFFFF) FOR [EK6]
GO
ALTER TABLE [dbo].[tbl_supplement] ADD  CONSTRAINT [DF_tbl_supplement_ED6]  DEFAULT ((0)) FOR [ED6]
GO
ALTER TABLE [dbo].[tbl_supplement] ADD  CONSTRAINT [DF_tbl_supplement_ES6]  DEFAULT ((0)) FOR [ES6]
GO
ALTER TABLE [dbo].[tbl_supplement] ADD  CONSTRAINT [DF_tbl_supplement_ET6]  DEFAULT (0xFFFFFFFF) FOR [ET6]
GO
ALTER TABLE [dbo].[tbl_TimeLimitInfo] ADD  CONSTRAINT [DF_tbl_TimeLimitInfo_AccountSerial]  DEFAULT ((0)) FOR [AccountSerial]
GO
ALTER TABLE [dbo].[tbl_TimeLimitInfo] ADD  CONSTRAINT [DF_tbl_TimeLimitInfo_TLStatus]  DEFAULT ((0)) FOR [TLStatus]
GO
ALTER TABLE [dbo].[tbl_TimeLimitInfo] ADD  CONSTRAINT [DF_tbl_TimeLimitInfo_Fatigue]  DEFAULT ((0)) FOR [Fatigue]
GO
ALTER TABLE [dbo].[tbl_TimeLimitInfo] ADD  CONSTRAINT [DF_tbl_TimeLimitInfo_LastLoginTime]  DEFAULT ((101010000)) FOR [LastLogoutTime]
GO
ALTER TABLE [dbo].[tbl_unit] ADD  CONSTRAINT [DF_tbl_unit_Serial]  DEFAULT ((-1)) FOR [Serial]
GO
ALTER TABLE [dbo].[tbl_unit] ADD  CONSTRAINT [DF_tbl_unit_DCK]  DEFAULT ((0)) FOR [DCK]
GO
ALTER TABLE [dbo].[tbl_unit] ADD  CONSTRAINT [DF_tbl_unit_F_0]  DEFAULT (0xFF) FOR [F_0]
GO
ALTER TABLE [dbo].[tbl_unit] ADD  CONSTRAINT [DF_tbl_unit_H_0]  DEFAULT (0xFF) FOR [H_0]
GO
ALTER TABLE [dbo].[tbl_unit] ADD  CONSTRAINT [DF_tbl_unit_U_0]  DEFAULT (0xFF) FOR [U_0]
GO
ALTER TABLE [dbo].[tbl_unit] ADD  CONSTRAINT [DF_tbl_unit_L_0]  DEFAULT (0xFF) FOR [L_0]
GO
ALTER TABLE [dbo].[tbl_unit] ADD  CONSTRAINT [DF_tbl_unit_A_0]  DEFAULT (0xFF) FOR [A_0]
GO
ALTER TABLE [dbo].[tbl_unit] ADD  CONSTRAINT [DF_tbl_unit_S_0]  DEFAULT (0xFF) FOR [S_0]
GO
ALTER TABLE [dbo].[tbl_unit] ADD  CONSTRAINT [DF_tbl_unit_B_0]  DEFAULT (0xFF) FOR [B_0]
GO
ALTER TABLE [dbo].[tbl_unit] ADD  CONSTRAINT [DF_tbl_unit_Gg_0]  DEFAULT ((100)) FOR [Gg_0]
GO
ALTER TABLE [dbo].[tbl_unit] ADD  CONSTRAINT [DF_tbl_unit_AB_0]  DEFAULT (0xFFFFFFFF) FOR [AB_0]
GO
ALTER TABLE [dbo].[tbl_unit] ADD  CONSTRAINT [DF_tbl_unit_SB_0]  DEFAULT (0xFFFFFFFF) FOR [SB_0]
GO
ALTER TABLE [dbo].[tbl_unit] ADD  CONSTRAINT [DF_tbl_unit_Sp0_0]  DEFAULT (0xFFFFFFFF) FOR [Sp0_0]
GO
ALTER TABLE [dbo].[tbl_unit] ADD  CONSTRAINT [DF_tbl_unit_Sp1_0]  DEFAULT (0xFFFFFFFF) FOR [Sp1_0]
GO
ALTER TABLE [dbo].[tbl_unit] ADD  CONSTRAINT [DF_tbl_unit_Sp2_0]  DEFAULT (0xFFFFFFFF) FOR [Sp2_0]
GO
ALTER TABLE [dbo].[tbl_unit] ADD  CONSTRAINT [DF_tbl_unit_Sp3_0]  DEFAULT (0xFFFFFFFF) FOR [Sp3_0]
GO
ALTER TABLE [dbo].[tbl_unit] ADD  CONSTRAINT [DF_tbl_unit_Sp4_0]  DEFAULT (0xFFFFFFFF) FOR [Sp4_0]
GO
ALTER TABLE [dbo].[tbl_unit] ADD  CONSTRAINT [DF_tbl_unit_Sp5_0]  DEFAULT (0xFFFFFFFF) FOR [Sp5_0]
GO
ALTER TABLE [dbo].[tbl_unit] ADD  CONSTRAINT [DF_tbl_unit_Sp6_0]  DEFAULT (0xFFFFFFFF) FOR [Sp6_0]
GO
ALTER TABLE [dbo].[tbl_unit] ADD  CONSTRAINT [DF_tbl_unit_Sp7_0]  DEFAULT (0xFFFFFFFF) FOR [Sp7_0]
GO
ALTER TABLE [dbo].[tbl_unit] ADD  CONSTRAINT [DF_tbl_unit_KF]  DEFAULT ((0)) FOR [KF_0]
GO
ALTER TABLE [dbo].[tbl_unit] ADD  CONSTRAINT [DF_tbl_unit_PF]  DEFAULT ((0)) FOR [PF_0]
GO
ALTER TABLE [dbo].[tbl_unit] ADD  CONSTRAINT [DF_tbl_unit_Cut_0]  DEFAULT ((0)) FOR [Cut_0]
GO
ALTER TABLE [dbo].[tbl_unit] ADD  CONSTRAINT [DF_tbl_unit_F_1]  DEFAULT (0xFF) FOR [F_1]
GO
ALTER TABLE [dbo].[tbl_unit] ADD  CONSTRAINT [DF_tbl_unit_H_1]  DEFAULT (0xFF) FOR [H_1]
GO
ALTER TABLE [dbo].[tbl_unit] ADD  CONSTRAINT [DF_tbl_unit_U_1]  DEFAULT (0xFF) FOR [U_1]
GO
ALTER TABLE [dbo].[tbl_unit] ADD  CONSTRAINT [DF_tbl_unit_L_1]  DEFAULT (0xFF) FOR [L_1]
GO
ALTER TABLE [dbo].[tbl_unit] ADD  CONSTRAINT [DF_tbl_unit_A_1]  DEFAULT (0xFF) FOR [A_1]
GO
ALTER TABLE [dbo].[tbl_unit] ADD  CONSTRAINT [DF_tbl_unit_S_1]  DEFAULT (0xFF) FOR [S_1]
GO
ALTER TABLE [dbo].[tbl_unit] ADD  CONSTRAINT [DF_tbl_unit_B_1]  DEFAULT (0xFF) FOR [B_1]
GO
ALTER TABLE [dbo].[tbl_unit] ADD  CONSTRAINT [DF_tbl_unit_Gg_1]  DEFAULT ((100)) FOR [Gg_1]
GO
ALTER TABLE [dbo].[tbl_unit] ADD  CONSTRAINT [DF_tbl_unit_AB_1]  DEFAULT (0xFFFFFFFF) FOR [AB_1]
GO
ALTER TABLE [dbo].[tbl_unit] ADD  CONSTRAINT [DF_tbl_unit_SB_1]  DEFAULT (0xFFFFFFFF) FOR [SB_1]
GO
ALTER TABLE [dbo].[tbl_unit] ADD  CONSTRAINT [DF_tbl_unit_Sp0_1]  DEFAULT (0xFFFFFFFF) FOR [Sp0_1]
GO
ALTER TABLE [dbo].[tbl_unit] ADD  CONSTRAINT [DF_tbl_unit_Sp1_1]  DEFAULT (0xFFFFFFFF) FOR [Sp1_1]
GO
ALTER TABLE [dbo].[tbl_unit] ADD  CONSTRAINT [DF_tbl_unit_Sp2_1]  DEFAULT (0xFFFFFFFF) FOR [Sp2_1]
GO
ALTER TABLE [dbo].[tbl_unit] ADD  CONSTRAINT [DF_tbl_unit_Sp3_1]  DEFAULT (0xFFFFFFFF) FOR [Sp3_1]
GO
ALTER TABLE [dbo].[tbl_unit] ADD  CONSTRAINT [DF_tbl_unit_Sp4_1]  DEFAULT (0xFFFFFFFF) FOR [Sp4_1]
GO
ALTER TABLE [dbo].[tbl_unit] ADD  CONSTRAINT [DF_tbl_unit_Sp5_1]  DEFAULT (0xFFFFFFFF) FOR [Sp5_1]
GO
ALTER TABLE [dbo].[tbl_unit] ADD  CONSTRAINT [DF_tbl_unit_Sp6_1]  DEFAULT (0xFFFFFFFF) FOR [Sp6_1]
GO
ALTER TABLE [dbo].[tbl_unit] ADD  CONSTRAINT [DF_tbl_unit_Sp7_1]  DEFAULT (0xFFFFFFFF) FOR [Sp7_1]
GO
ALTER TABLE [dbo].[tbl_unit] ADD  CONSTRAINT [DF_tbl_unit_KF_01]  DEFAULT ((0)) FOR [KF_1]
GO
ALTER TABLE [dbo].[tbl_unit] ADD  CONSTRAINT [DF_tbl_unit_PF_01]  DEFAULT ((0)) FOR [PF_1]
GO
ALTER TABLE [dbo].[tbl_unit] ADD  CONSTRAINT [DF_tbl_unit_Cut_01]  DEFAULT ((0)) FOR [Cut_1]
GO
ALTER TABLE [dbo].[tbl_unit] ADD  CONSTRAINT [DF_tbl_unit_F_2]  DEFAULT (0xFF) FOR [F_2]
GO
ALTER TABLE [dbo].[tbl_unit] ADD  CONSTRAINT [DF_tbl_unit_H_2]  DEFAULT (0xFF) FOR [H_2]
GO
ALTER TABLE [dbo].[tbl_unit] ADD  CONSTRAINT [DF_tbl_unit_U_2]  DEFAULT (0xFF) FOR [U_2]
GO
ALTER TABLE [dbo].[tbl_unit] ADD  CONSTRAINT [DF_tbl_unit_L_2]  DEFAULT (0xFF) FOR [L_2]
GO
ALTER TABLE [dbo].[tbl_unit] ADD  CONSTRAINT [DF_tbl_unit_A_2]  DEFAULT (0xFF) FOR [A_2]
GO
ALTER TABLE [dbo].[tbl_unit] ADD  CONSTRAINT [DF_tbl_unit_S_2]  DEFAULT (0xFF) FOR [S_2]
GO
ALTER TABLE [dbo].[tbl_unit] ADD  CONSTRAINT [DF_tbl_unit_B_2]  DEFAULT (0xFF) FOR [B_2]
GO
ALTER TABLE [dbo].[tbl_unit] ADD  CONSTRAINT [DF_tbl_unit_Gg_2]  DEFAULT ((100)) FOR [Gg_2]
GO
ALTER TABLE [dbo].[tbl_unit] ADD  CONSTRAINT [DF_tbl_unit_AB_2]  DEFAULT (0xFFFFFFFF) FOR [AB_2]
GO
ALTER TABLE [dbo].[tbl_unit] ADD  CONSTRAINT [DF_tbl_unit_SB_2]  DEFAULT (0xFFFFFFFF) FOR [SB_2]
GO
ALTER TABLE [dbo].[tbl_unit] ADD  CONSTRAINT [DF_tbl_unit_Sp0_2]  DEFAULT (0xFFFFFFFF) FOR [Sp0_2]
GO
ALTER TABLE [dbo].[tbl_unit] ADD  CONSTRAINT [DF_tbl_unit_Sp1_2]  DEFAULT (0xFFFFFFFF) FOR [Sp1_2]
GO
ALTER TABLE [dbo].[tbl_unit] ADD  CONSTRAINT [DF_tbl_unit_Sp2_2]  DEFAULT (0xFFFFFFFF) FOR [Sp2_2]
GO
ALTER TABLE [dbo].[tbl_unit] ADD  CONSTRAINT [DF_tbl_unit_Sp3_2]  DEFAULT (0xFFFFFFFF) FOR [Sp3_2]
GO
ALTER TABLE [dbo].[tbl_unit] ADD  CONSTRAINT [DF_tbl_unit_Sp4_2]  DEFAULT (0xFFFFFFFF) FOR [Sp4_2]
GO
ALTER TABLE [dbo].[tbl_unit] ADD  CONSTRAINT [DF_tbl_unit_Sp5_2]  DEFAULT (0xFFFFFFFF) FOR [Sp5_2]
GO
ALTER TABLE [dbo].[tbl_unit] ADD  CONSTRAINT [DF_tbl_unit_Sp6_2]  DEFAULT (0xFFFFFFFF) FOR [Sp6_2]
GO
ALTER TABLE [dbo].[tbl_unit] ADD  CONSTRAINT [DF_tbl_unit_Sp7_2]  DEFAULT (0xFFFFFFFF) FOR [Sp7_2]
GO
ALTER TABLE [dbo].[tbl_unit] ADD  CONSTRAINT [DF_tbl_unit_KF_01_1]  DEFAULT ((0)) FOR [KF_2]
GO
ALTER TABLE [dbo].[tbl_unit] ADD  CONSTRAINT [DF_tbl_unit_PF_01_1]  DEFAULT ((0)) FOR [PF_2]
GO
ALTER TABLE [dbo].[tbl_unit] ADD  CONSTRAINT [DF_tbl_unit_Cut_01_1]  DEFAULT ((0)) FOR [Cut_2]
GO
ALTER TABLE [dbo].[tbl_unit] ADD  CONSTRAINT [DF_tbl_unit_F_3]  DEFAULT (0xFF) FOR [F_3]
GO
ALTER TABLE [dbo].[tbl_unit] ADD  CONSTRAINT [DF_tbl_unit_H_3]  DEFAULT (0xFF) FOR [H_3]
GO
ALTER TABLE [dbo].[tbl_unit] ADD  CONSTRAINT [DF_tbl_unit_U_3]  DEFAULT (0xFF) FOR [U_3]
GO
ALTER TABLE [dbo].[tbl_unit] ADD  CONSTRAINT [DF_tbl_unit_L_3]  DEFAULT (0xFF) FOR [L_3]
GO
ALTER TABLE [dbo].[tbl_unit] ADD  CONSTRAINT [DF_tbl_unit_A_3]  DEFAULT (0xFF) FOR [A_3]
GO
ALTER TABLE [dbo].[tbl_unit] ADD  CONSTRAINT [DF_tbl_unit_S_3]  DEFAULT (0xFF) FOR [S_3]
GO
ALTER TABLE [dbo].[tbl_unit] ADD  CONSTRAINT [DF_tbl_unit_B_3]  DEFAULT (0xFF) FOR [B_3]
GO
ALTER TABLE [dbo].[tbl_unit] ADD  CONSTRAINT [DF_tbl_unit_Gg_3]  DEFAULT ((100)) FOR [Gg_3]
GO
ALTER TABLE [dbo].[tbl_unit] ADD  CONSTRAINT [DF_tbl_unit_AB_3]  DEFAULT (0xFFFFFFFF) FOR [AB_3]
GO
ALTER TABLE [dbo].[tbl_unit] ADD  CONSTRAINT [DF_tbl_unit_SB_3]  DEFAULT (0xFFFFFFFF) FOR [SB_3]
GO
ALTER TABLE [dbo].[tbl_unit] ADD  CONSTRAINT [DF_tbl_unit_Sp0_3]  DEFAULT (0xFFFFFFFF) FOR [Sp0_3]
GO
ALTER TABLE [dbo].[tbl_unit] ADD  CONSTRAINT [DF_tbl_unit_Sp1_3]  DEFAULT (0xFFFFFFFF) FOR [Sp1_3]
GO
ALTER TABLE [dbo].[tbl_unit] ADD  CONSTRAINT [DF_tbl_unit_Sp2_3]  DEFAULT (0xFFFFFFFF) FOR [Sp2_3]
GO
ALTER TABLE [dbo].[tbl_unit] ADD  CONSTRAINT [DF_tbl_unit_Sp3_3]  DEFAULT (0xFFFFFFFF) FOR [Sp3_3]
GO
ALTER TABLE [dbo].[tbl_unit] ADD  CONSTRAINT [DF_tbl_unit_Sp4_3]  DEFAULT (0xFFFFFFFF) FOR [Sp4_3]
GO
ALTER TABLE [dbo].[tbl_unit] ADD  CONSTRAINT [DF_tbl_unit_Sp5_3]  DEFAULT (0xFFFFFFFF) FOR [Sp5_3]
GO
ALTER TABLE [dbo].[tbl_unit] ADD  CONSTRAINT [DF_tbl_unit_Sp6_3]  DEFAULT (0xFFFFFFFF) FOR [Sp6_3]
GO
ALTER TABLE [dbo].[tbl_unit] ADD  CONSTRAINT [DF_tbl_unit_Sp7_3]  DEFAULT (0xFFFFFFFF) FOR [Sp7_3]
GO
ALTER TABLE [dbo].[tbl_unit] ADD  CONSTRAINT [DF_tbl_unit_KF_01_2]  DEFAULT ((0)) FOR [KF_3]
GO
ALTER TABLE [dbo].[tbl_unit] ADD  CONSTRAINT [DF_tbl_unit_PF_01_2]  DEFAULT ((0)) FOR [PF_3]
GO
ALTER TABLE [dbo].[tbl_unit] ADD  CONSTRAINT [DF_tbl_unit_Cut_01_2]  DEFAULT ((0)) FOR [Cut_3]
GO
ALTER TABLE [dbo].[tbl_unitlog] ADD  CONSTRAINT [DF_tbl_unitlog_Name]  DEFAULT ('*') FOR [Name]
GO
ALTER TABLE [dbo].[tbl_unitlog] ADD  CONSTRAINT [DF_tbl_unitlog_LogDate]  DEFAULT (getdate()) FOR [LogDate]
GO
ALTER TABLE [dbo].[tbl_userinterface] ADD  CONSTRAINT [DF_tbl_userinterface_Serial]  DEFAULT ((-1)) FOR [Serial]
GO
ALTER TABLE [dbo].[tbl_userinterface] ADD  CONSTRAINT [DF_tbl_userinterface_DCK]  DEFAULT ((0)) FOR [DCK]
GO
ALTER TABLE [dbo].[tbl_userinterface] ADD  CONSTRAINT [DF_tbl_userinterface_LB0]  DEFAULT (0xFFFF) FOR [LB0]
GO
ALTER TABLE [dbo].[tbl_userinterface] ADD  CONSTRAINT [DF_tbl_userinterface_LB1]  DEFAULT (0xFFFF) FOR [LB1]
GO
ALTER TABLE [dbo].[tbl_userinterface] ADD  CONSTRAINT [DF_tbl_userinterface_LB2]  DEFAULT (0xFFFF) FOR [LB2]
GO
ALTER TABLE [dbo].[tbl_userinterface] ADD  CONSTRAINT [DF_tbl_userinterface_LB3]  DEFAULT (0xFFFF) FOR [LB3]
GO
ALTER TABLE [dbo].[tbl_userinterface] ADD  CONSTRAINT [DF_tbl_userinterface_LB4]  DEFAULT (0xFFFF) FOR [LB4]
GO
ALTER TABLE [dbo].[tbl_userinterface] ADD  CONSTRAINT [DF_tbl_userinterface_LB5]  DEFAULT (0xFFFF) FOR [LB5]
GO
ALTER TABLE [dbo].[tbl_userinterface] ADD  CONSTRAINT [DF_tbl_userinterface_LB6]  DEFAULT (0xFFFF) FOR [LB6]
GO
ALTER TABLE [dbo].[tbl_userinterface] ADD  CONSTRAINT [DF_tbl_userinterface_LB7]  DEFAULT (0xFFFF) FOR [LB7]
GO
ALTER TABLE [dbo].[tbl_userinterface] ADD  CONSTRAINT [DF_tbl_userinterface_LB8]  DEFAULT (0xFFFF) FOR [LB8]
GO
ALTER TABLE [dbo].[tbl_userinterface] ADD  CONSTRAINT [DF_tbl_userinterface_LB9]  DEFAULT (0xFFFF) FOR [LB9]
GO
ALTER TABLE [dbo].[tbl_userinterface] ADD  CONSTRAINT [DF_tbl_userinterface_LB10]  DEFAULT (0xFFFF) FOR [LB10]
GO
ALTER TABLE [dbo].[tbl_userinterface] ADD  CONSTRAINT [DF_tbl_userinterface_LB11]  DEFAULT (0xFFFF) FOR [LB11]
GO
ALTER TABLE [dbo].[tbl_userinterface] ADD  CONSTRAINT [DF_tbl_userinterface_LB12]  DEFAULT (0xFFFF) FOR [LB12]
GO
ALTER TABLE [dbo].[tbl_userinterface] ADD  CONSTRAINT [DF_tbl_userinterface_LB13]  DEFAULT (0xFFFF) FOR [LB13]
GO
ALTER TABLE [dbo].[tbl_userinterface] ADD  CONSTRAINT [DF_tbl_userinterface_LB14]  DEFAULT (0xFFFF) FOR [LB14]
GO
ALTER TABLE [dbo].[tbl_userinterface] ADD  CONSTRAINT [DF_tbl_userinterface_LB15]  DEFAULT (0xFFFF) FOR [LB15]
GO
ALTER TABLE [dbo].[tbl_userinterface] ADD  CONSTRAINT [DF_tbl_userinterface_LB16]  DEFAULT (0xFFFF) FOR [LB16]
GO
ALTER TABLE [dbo].[tbl_userinterface] ADD  CONSTRAINT [DF_tbl_userinterface_LB17]  DEFAULT (0xFFFF) FOR [LB17]
GO
ALTER TABLE [dbo].[tbl_userinterface] ADD  CONSTRAINT [DF_tbl_userinterface_LB18]  DEFAULT (0xFFFF) FOR [LB18]
GO
ALTER TABLE [dbo].[tbl_userinterface] ADD  CONSTRAINT [DF_tbl_userinterface_LB19]  DEFAULT (0xFFFF) FOR [LB19]
GO
ALTER TABLE [dbo].[tbl_userinterface] ADD  CONSTRAINT [DF_tbl_userinterface_LB20]  DEFAULT (0xFFFF) FOR [LB20]
GO
ALTER TABLE [dbo].[tbl_userinterface] ADD  CONSTRAINT [DF_tbl_userinterface_LB21]  DEFAULT (0xFFFF) FOR [LB21]
GO
ALTER TABLE [dbo].[tbl_userinterface] ADD  CONSTRAINT [DF_tbl_userinterface_LB22]  DEFAULT (0xFFFF) FOR [LB22]
GO
ALTER TABLE [dbo].[tbl_userinterface] ADD  CONSTRAINT [DF_tbl_userinterface_LB23]  DEFAULT (0xFFFF) FOR [LB23]
GO
ALTER TABLE [dbo].[tbl_userinterface] ADD  CONSTRAINT [DF_tbl_userinterface_LB24]  DEFAULT (0xFFFF) FOR [LB24]
GO
ALTER TABLE [dbo].[tbl_userinterface] ADD  CONSTRAINT [DF_tbl_userinterface_LB25]  DEFAULT (0xFFFF) FOR [LB25]
GO
ALTER TABLE [dbo].[tbl_userinterface] ADD  CONSTRAINT [DF_tbl_userinterface_LB26]  DEFAULT (0xFFFF) FOR [LB26]
GO
ALTER TABLE [dbo].[tbl_userinterface] ADD  CONSTRAINT [DF_tbl_userinterface_LB27]  DEFAULT (0xFFFF) FOR [LB27]
GO
ALTER TABLE [dbo].[tbl_userinterface] ADD  CONSTRAINT [DF_tbl_userinterface_LB28]  DEFAULT (0xFFFF) FOR [LB28]
GO
ALTER TABLE [dbo].[tbl_userinterface] ADD  CONSTRAINT [DF_tbl_userinterface_LB29]  DEFAULT (0xFFFF) FOR [LB29]
GO
ALTER TABLE [dbo].[tbl_userinterface] ADD  CONSTRAINT [DF_tbl_userinterface_LB30]  DEFAULT (0xFFFF) FOR [LB30]
GO
ALTER TABLE [dbo].[tbl_userinterface] ADD  CONSTRAINT [DF_tbl_userinterface_LB31]  DEFAULT (0xFFFF) FOR [LB31]
GO
ALTER TABLE [dbo].[tbl_userinterface] ADD  CONSTRAINT [DF_tbl_userinterface_LB32]  DEFAULT (0xFFFF) FOR [LB32]
GO
ALTER TABLE [dbo].[tbl_userinterface] ADD  CONSTRAINT [DF_tbl_userinterface_LB33]  DEFAULT (0xFFFF) FOR [LB33]
GO
ALTER TABLE [dbo].[tbl_userinterface] ADD  CONSTRAINT [DF_tbl_userinterface_LB34]  DEFAULT (0xFFFF) FOR [LB34]
GO
ALTER TABLE [dbo].[tbl_userinterface] ADD  CONSTRAINT [DF_tbl_userinterface_LB35]  DEFAULT (0xFFFF) FOR [LB35]
GO
ALTER TABLE [dbo].[tbl_userinterface] ADD  CONSTRAINT [DF_tbl_userinterface_LB36]  DEFAULT (0xFFFF) FOR [LB36]
GO
ALTER TABLE [dbo].[tbl_userinterface] ADD  CONSTRAINT [DF_tbl_userinterface_LB37]  DEFAULT (0xFFFF) FOR [LB37]
GO
ALTER TABLE [dbo].[tbl_userinterface] ADD  CONSTRAINT [DF_tbl_userinterface_LB38]  DEFAULT (0xFFFF) FOR [LB38]
GO
ALTER TABLE [dbo].[tbl_userinterface] ADD  CONSTRAINT [DF_tbl_userinterface_LB39]  DEFAULT (0xFFFF) FOR [LB39]
GO
ALTER TABLE [dbo].[tbl_userinterface] ADD  CONSTRAINT [DF_tbl_userinterface_LB40]  DEFAULT (0xFFFF) FOR [LB40]
GO
ALTER TABLE [dbo].[tbl_userinterface] ADD  CONSTRAINT [DF_tbl_userinterface_LB41]  DEFAULT (0xFFFF) FOR [LB41]
GO
ALTER TABLE [dbo].[tbl_userinterface] ADD  CONSTRAINT [DF_tbl_userinterface_LB42]  DEFAULT (0xFFFF) FOR [LB42]
GO
ALTER TABLE [dbo].[tbl_userinterface] ADD  CONSTRAINT [DF_tbl_userinterface_LB43]  DEFAULT (0xFFFF) FOR [LB43]
GO
ALTER TABLE [dbo].[tbl_userinterface] ADD  CONSTRAINT [DF_tbl_userinterface_LB44]  DEFAULT (0xFFFF) FOR [LB44]
GO
ALTER TABLE [dbo].[tbl_userinterface] ADD  CONSTRAINT [DF_tbl_userinterface_LB45]  DEFAULT (0xFFFF) FOR [LB45]
GO
ALTER TABLE [dbo].[tbl_userinterface] ADD  CONSTRAINT [DF_tbl_userinterface_LB46]  DEFAULT (0xFFFF) FOR [LB46]
GO
ALTER TABLE [dbo].[tbl_userinterface] ADD  CONSTRAINT [DF_tbl_userinterface_LB47]  DEFAULT (0xFFFF) FOR [LB47]
GO
ALTER TABLE [dbo].[tbl_userinterface] ADD  CONSTRAINT [DF_tbl_userinterface_LB48]  DEFAULT (0xFFFF) FOR [LB48]
GO
ALTER TABLE [dbo].[tbl_userinterface] ADD  CONSTRAINT [DF_tbl_userinterface_LB49]  DEFAULT (0xFFFF) FOR [LB49]
GO
ALTER TABLE [dbo].[tbl_userinterface] ADD  CONSTRAINT [DF_tbl_userinterface_CF0]  DEFAULT (0xFFFFFFFF) FOR [DCF0]
GO
ALTER TABLE [dbo].[tbl_userinterface] ADD  CONSTRAINT [DF_tbl_userinterface_CF1]  DEFAULT (0xFFFFFFFF) FOR [DCF1]
GO
ALTER TABLE [dbo].[tbl_userinterface] ADD  CONSTRAINT [DF_tbl_userinterface_CF2]  DEFAULT (0xFFFFFFFF) FOR [DCF2]
GO
ALTER TABLE [dbo].[tbl_userinterface] ADD  CONSTRAINT [DF_tbl_userinterface_CF3]  DEFAULT (0xFFFFFFFF) FOR [DCF3]
GO
ALTER TABLE [dbo].[tbl_userinterface] ADD  CONSTRAINT [DF_tbl_userinterface_DCF4]  DEFAULT (0xFFFFFFFF) FOR [DCF4]
GO
ALTER TABLE [dbo].[tbl_userinterface] ADD  CONSTRAINT [DF_tbl_userinterface_DCF5]  DEFAULT (0xFFFFFFFF) FOR [DCF5]
GO
ALTER TABLE [dbo].[tbl_userinterface] ADD  CONSTRAINT [DF_tbl_userinterface_DCF6]  DEFAULT (0xFFFFFFFF) FOR [DCF6]
GO
ALTER TABLE [dbo].[tbl_userinterface] ADD  CONSTRAINT [DF_tbl_userinterface_DCF7]  DEFAULT (0xFFFFFFFF) FOR [DCF7]
GO
ALTER TABLE [dbo].[tbl_userinterface] ADD  CONSTRAINT [DF_tbl_userinterface_HCF0]  DEFAULT (0xFFFFFFFF) FOR [HCF0]
GO
ALTER TABLE [dbo].[tbl_userinterface] ADD  CONSTRAINT [DF_tbl_userinterface_HCF1]  DEFAULT (0xFFFFFFFF) FOR [HCF1]
GO
ALTER TABLE [dbo].[tbl_userinterface] ADD  CONSTRAINT [DF_tbl_userinterface_HCF2]  DEFAULT (0xFFFFFFFF) FOR [HCF2]
GO
ALTER TABLE [dbo].[tbl_userinterface] ADD  CONSTRAINT [DF_tbl_userinterface_HCF3]  DEFAULT (0xFFFFFFFF) FOR [HCF3]
GO
ALTER TABLE [dbo].[tbl_userinterface] ADD  CONSTRAINT [DF_tbl_userinterface_HCF4]  DEFAULT (0xFFFFFFFF) FOR [HCF4]
GO
ALTER TABLE [dbo].[tbl_userinterface] ADD  CONSTRAINT [DF_tbl_userinterface_HCF5]  DEFAULT (0xFFFFFFFF) FOR [HCF5]
GO
ALTER TABLE [dbo].[tbl_userinterface] ADD  CONSTRAINT [DF_tbl_userinterface_HCF6]  DEFAULT (0xFFFFFFFF) FOR [HCF6]
GO
ALTER TABLE [dbo].[tbl_userinterface] ADD  CONSTRAINT [DF_tbl_userinterface_HCF7]  DEFAULT (0xFFFFFFFF) FOR [HCF7]
GO
ALTER TABLE [dbo].[tbl_userinterface] ADD  CONSTRAINT [DF_tbl_userinterface_SkillWin0_1]  DEFAULT ((0)) FOR [SkillWin0]
GO
ALTER TABLE [dbo].[tbl_userinterface] ADD  CONSTRAINT [DF_tbl_userinterface_SKillWin1_1]  DEFAULT ((0)) FOR [SKillWin1]
GO
ALTER TABLE [dbo].[tbl_userinterface] ADD  CONSTRAINT [DF_tbl_userinterface_ForceWin0_1]  DEFAULT ((0)) FOR [ForceWin0]
GO
ALTER TABLE [dbo].[tbl_userinterface] ADD  CONSTRAINT [DF_tbl_userinterface_ForceWin1_1]  DEFAULT ((0)) FOR [ForceWin1]
GO
ALTER TABLE [dbo].[tbl_userinterface] ADD  CONSTRAINT [DF_tbl_userinterface_CharWin0_1]  DEFAULT ((0)) FOR [CharWin0]
GO
ALTER TABLE [dbo].[tbl_userinterface] ADD  CONSTRAINT [DF_tbl_userinterface_CharWin1_1]  DEFAULT ((0)) FOR [CharWin1]
GO
ALTER TABLE [dbo].[tbl_userinterface] ADD  CONSTRAINT [DF_tbl_userinterface_AnimusWin0_1]  DEFAULT ((0)) FOR [AnimusWin0]
GO
ALTER TABLE [dbo].[tbl_userinterface] ADD  CONSTRAINT [DF_tbl_userinterface_AnimusWin1_1]  DEFAULT ((0)) FOR [AnimusWin1]
GO
ALTER TABLE [dbo].[tbl_userinterface] ADD  CONSTRAINT [DF_tbl_userinterface_InvenWin_1]  DEFAULT ((0)) FOR [InvenWin]
GO
ALTER TABLE [dbo].[tbl_userinterface] ADD  CONSTRAINT [DF_tbl_userinterface_InvenBag0_1]  DEFAULT ((0)) FOR [InvenBag0]
GO
ALTER TABLE [dbo].[tbl_userinterface] ADD  CONSTRAINT [DF_tbl_userinterface_InvenBag1_1]  DEFAULT ((0)) FOR [InvenBag1]
GO
ALTER TABLE [dbo].[tbl_userinterface] ADD  CONSTRAINT [DF_tbl_userinterface_InvenBag2_1]  DEFAULT ((0)) FOR [InvenBag2]
GO
ALTER TABLE [dbo].[tbl_userinterface] ADD  CONSTRAINT [DF_tbl_userinterface_InvenBag3_1]  DEFAULT ((0)) FOR [InvenBag3]
GO
ALTER TABLE [dbo].[tbl_userinterface] ADD  CONSTRAINT [DF_tbl_userinterface_InvenBag4_1]  DEFAULT ((0)) FOR [InvenBag4]
GO
ALTER TABLE [dbo].[tbl_userinterface] ADD  CONSTRAINT [DF_tbl_userinterface_LBLock]  DEFAULT ((0)) FOR [LBLock]
GO
ALTER TABLE [dbo].[tbl_utgroupiteminfo] ADD  CONSTRAINT [DF_tbl_utgroupiteminfo_dck]  DEFAULT ((1)) FOR [dck]
GO
ALTER TABLE [dbo].[tbl_utsingleiteminfo] ADD  CONSTRAINT [DF_tbl_utsingleiteminfo_dck]  DEFAULT ((1)) FOR [dck]
GO
ALTER TABLE [dbo].[tbl_utsingleiteminfo] ADD  CONSTRAINT [DF__tbl_utsinglei__s__62E27A01]  DEFAULT ((0)) FOR [s]
GO
ALTER TABLE [dbo].[tbl_utsingleiteminfo] ADD  CONSTRAINT [DF__tbl_utsinglei__t__63D69E3A]  DEFAULT (0xFFFFFFFF) FOR [t]
GO
ALTER TABLE [dbo].[tbl_WeeklyGuildPVPPointSum] ADD  CONSTRAINT [DF_tbl_WeeklyGuildPVPPointSum_killpvppoint]  DEFAULT ((0)) FOR [killpvppoint]
GO
ALTER TABLE [dbo].[tbl_WeeklyGuildPVPPointSum] ADD  CONSTRAINT [DF_tbl_WeeklyGuildPVPPointSum_guildbattlepvppoint]  DEFAULT ((0)) FOR [guildbattlepvppoint]
GO
/****** Object:  StoredProcedure [dbo].[pAdd_PvpPoint]    Script Date: 12/01/2026 16:57:20 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO



--
CREATE PROCEDURE [dbo].[pAdd_PvpPoint]
@serial int,
@point int,
@cashbag int
AS
update [dbo].[tbl_general] set PvpPoint=PvpPoint+@point, PvpCashBag=PvpCashBag+@cashbag where serial=@serial






GO
/****** Object:  StoredProcedure [dbo].[pCopy_Base]    Script Date: 12/01/2026 16:57:20 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO


create procedure [dbo].[pCopy_Base]  
@srcserial int,  
@destserial int  
as
update [dbo].[tbl_Base]
set
[Race] = b.[Race],
[Class] = b.[Class],
[Lv] = b.[Lv],
[Dalant] = b.[Dalant],
[Gold] = b.[Gold],
[BaseShape] = b.[BaseShape],
[EK0] = b.[EK0], [EU0] = b.[EU0], [EK1] = b.[EK1], [EU1] = b.[EU1],
[EK2] = b.[EK2], [EU2] = b.[EU2], [EK3] = b.[EK3], [EU3] = b.[EU3],
[EK4] = b.[EK4], [EU4] = b.[EU4], [EK5] = b.[EK5], [EU5] = b.[EU5],
[EK6] = b.[EK6], [EU6] = b.[EU6], [EK7] = b.[EK7], [EU7] = b.[EU7],
[ES0] = b.[ES0], [ET0] = b.[ET0], [ES1] = b.[ES1], [ET1] = b.[ET1],
[ES2] = b.[ES2], [ET2] = b.[ET2], [ES3] = b.[ES3], [ET3] = b.[ET3],
[ES4] = b.[ES4], [ET4] = b.[ET4], [ES5] = b.[ES5], [ET5] = b.[ET5],
[ES6] = b.[ES6], [ET6] = b.[ET6], [ES7] = b.[ES7], [ET7] = b.[ET7]
from (select * from [dbo].[tbl_Base] where [Serial]=@srcserial) as b
where [dbo].[tbl_base].[Serial]=@destserial





GO
/****** Object:  StoredProcedure [dbo].[pCopy_General]    Script Date: 12/01/2026 16:57:20 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO



create procedure [dbo].[pCopy_General]
@srcserial int,  
@destserial int  
as
update [dbo].[tbl_general] set
[Exp] = g.[Exp], [LossExp] = g.[LossExp],
[HP] = g.[HP], [FP] = g.[FP], [SP] = g.[SP], [DP] = g.[DP],
[Map] = g.[Map], [X] = g.[X], [Y] = g.[Y], [Z] = g.[Z],
[BagNum] = g.[BagNum],
[EK0] = g.[EK0], [ED0] = g.[ED0], [EK1] = g.[EK1], [ED1] = g.[ED1],
[EK2] = g.[EK2], [ED2] = g.[ED2], [EK3] = g.[EK3], [ED3] = g.[ED3],
[EK4] = g.[EK4], [ED4] = g.[ED4], [EK5] = g.[EK5], [ED5] = g.[ED5],
[F0] = g.[F0], [F1] = g.[F1], [F2] = g.[F2], [F3] = g.[F3], [F4] = g.[F4],
[F5] = g.[F5], [F6] = g.[F6], [F7] = g.[F7], [F8] = g.[F8], [F9] = g.[F9],
[F10] = g.[F10], [F11] = g.[F11], [F12] = g.[F12], [F13] = g.[F13], [F14] = g.[F14],
[F15] = g.[F15], [F16] = g.[F16], [F17] = g.[F17], [F18] = g.[F18], [F19] = g.[F19],
[F20] = g.[F20], [F21] = g.[F21], [F22] = g.[F22], [F23] = g.[F23], [F24] = g.[F24],
[F25] = g.[F25], [F26] = g.[F26], [F27] = g.[F27], [F28] = g.[F28], [F29] = g.[F29],
[F30] = g.[F30], [F31] = g.[F31], [F32] = g.[F32], [F33] = g.[F33], [F34] = g.[F34],
[F35] = g.[F35], [F36] = g.[F36], [F37] = g.[F37], [F38] = g.[F38], [F39] = g.[F39],
[F40] = g.[F40], [F41] = g.[F41], [F42] = g.[F42], [F43] = g.[F43], [F44] = g.[F44],
[F45] = g.[F45], [F46] = g.[F46], [F47] = g.[F47], [F48] = g.[F48], [F49] = g.[F49],
[F50] = g.[F50], [F51] = g.[F51], [F52] = g.[F52], [F53] = g.[F53], [F54] = g.[F54],
[F55] = g.[F55], [F56] = g.[F56], [F57] = g.[F57], [F58] = g.[F58], [F59] = g.[F59],
[F60] = g.[F60], [F61] = g.[F61], [F62] = g.[F62], [F63] = g.[F63], [F64] = g.[F64],
[F65] = g.[F65], [F66] = g.[F66], [F67] = g.[F67], [F68] = g.[F68], [F69] = g.[F69],
[F70] = g.[F70], [F71] = g.[F71], [F72] = g.[F72], [F73] = g.[F73], [F74] = g.[F74],
[F75] = g.[F75], [F76] = g.[F76], [F77] = g.[F77], [F78] = g.[F78], [F79] = g.[F79],
[F80] = g.[F80], [F81] = g.[F81], [F82] = g.[F82], [F83] = g.[F83], [F84] = g.[F84],
[F85] = g.[F85], [F86] = g.[F86], [F87] = g.[F87], [F88] = g.[F88], [F89] = g.[F89],
[F90] = g.[F90], [F91] = g.[F91], [F92] = g.[F92], [F93] = g.[F93], [F94] = g.[F94],
[F95] = g.[F95], [F96] = g.[F96], [F97] = g.[F97], [F98] = g.[F98], [F99] = g.[F99],
[AK0] = g.[AK0], [AD0] = g.[AD0], [AP0] = g.[AP0], [AK1] = g.[AK1], [AD1] = g.[AD1], [AP1] = g.[AP1],
[AK2] = g.[AK2], [AD2] = g.[AD2], [AP2] = g.[AP2], [AK3] = g.[AK3], [AD3] = g.[AD3], [AP3] = g.[AP3],
[WM0] = g.[WM0], [WM1] = g.[WM1], [DM] = g.[DM], [PM] = g.[PM], [MI0] = g.[MI0], [MI1] = g.[MI1], [MI2] = g.[MI2], [SR] = g.[SR],
[FM0] = g.[FM0], [FM1] = g.[FM1], [FM2] = g.[FM2], [FM3] = g.[FM3], [FM4] = g.[FM4],
[FM5] = g.[FM5], [FM6] = g.[FM6], [FM7] = g.[FM7], [FM8] = g.[FM8], [FM9] = g.[FM9],
[FM10] = g.[FM10], [FM11] = g.[FM11], [FM12] = g.[FM12], [FM13] = g.[FM13], [FM14] = g.[FM14],
[FM15] = g.[FM15], [FM16] = g.[FM16], [FM17] = g.[FM17], [FM18] = g.[FM18], [FM19] = g.[FM19],
[FM20] = g.[FM20], [FM21] = g.[FM21], [FM22] = g.[FM22], [FM23] = g.[FM23],
[SM0] = g.[SM0], [SM1] = g.[SM1], [SM2] = g.[SM2], [SM3] = g.[SM3], [SM4] = g.[SM4],
[SM5] = g.[SM5], [SM6] = g.[SM6], [SM7] = g.[SM7], [SM8] = g.[SM8], [SM9] = g.[SM9],
[SM10] = g.[SM10], [SM11] = g.[SM11], [SM12] = g.[SM12], [SM13] = g.[SM13], [SM14] = g.[SM14],
[SM15] = g.[SM15], [SM16] = g.[SM16], [SM17] = g.[SM17], [SM18] = g.[SM18], [SM19] = g.[SM19],
[SM20] = g.[SM20], [SM21] = g.[SM21], [SM22] = g.[SM22], [SM23] = g.[SM23], [SM24] = g.[SM24],
[SM25] = g.[SM25], [SM26] = g.[SM26], [SM27] = g.[SM27], [SM28] = g.[SM28], [SM29] = g.[SM29],
[SM30] = g.[SM30], [SM31] = g.[SM31], [SM32] = g.[SM32], [SM33] = g.[SM33], [SM34] = g.[SM34],
[SM35] = g.[SM35], [SM36] = g.[SM36], [SM37] = g.[SM37], [SM38] = g.[SM38], [SM39] = g.[SM39],
[SM40] = g.[SM40], [SM41] = g.[SM41], [SM42] = g.[SM42], [SM43] = g.[SM43], [SM44] = g.[SM44],
[SM45] = g.[SM45], [SM46] = g.[SM46], [SM47] = g.[SM47], [SM48] = g.[SM48], [SM49] = g.[SM49],
[LeftResList] = g.[LeftResList],
[Class0] = g.[Class0], [Class1] = g.[Class1], [Class2] = g.[Class2],
[ClassInitCnt] = g.[ClassInitCnt], [LastClassGrade] = g.[LastClassGrade], [PvpPoint] = g.[PvpPoint], [PvpCashBag] = g.[PvpCashBag],
[BindMapCode] = g.[BindMapCode], [BindDummy] = g.[BindDummy],
[GuildSerial] = g.[GuildSerial], [GuildGrade] = g.[GuildGrade], [GuildRank] = g.[GuildRank],
[PvP_0] = g.[PvP_0], [PK_0] = g.[PK_0], [PvP_1] = g.[PvP_1], [PK_1] = g.[PK_1], [PvP_2] = g.[PvP_2], [PK_2] = g.[PK_2],
[CharacterGrade] = g.[CharacterGrade],
[Die_0] = g.[Die_0], [Die_1] = g.[Die_1], [Die_2] = g.[Die_2],
[tmRaceBossVote] = g.[tmRaceBossVote], [TakeLastMentalTicket] = g.[TakeLastMentalTicket], [TakeLastCriTicket] = g.[TakeLastCriTicket], [RDDelay] = g.[RDDelay],
[ES0] = g.[ES0], [ET0] = g.[ET0], [ES1] = g.[ES1], [ET1] = g.[ET1], [ES2] = g.[ES2], [ET2] = g.[ET2],
[ES3] = g.[ES3], [ET3] = g.[ET3], [ES4] = g.[ES4], [ET4] = g.[ET4], [ES5] = g.[ES5], [ET5] = g.[ET5],
[AS0] = g.[AS0], [AS1] = g.[AS1], [AS2] = g.[AS2], [AS3] = g.[AS3],
[FS0] = g.[FS0], [FS1] = g.[FS1], [FS2] = g.[FS2], [FS3] = g.[FS3], [FS4] = g.[FS4],
[FS5] = g.[FS5], [FS6] = g.[FS6], [FS7] = g.[FS7], [FS8] = g.[FS8], [FS9] = g.[FS9],
[FS10] = g.[FS10], [FS11] = g.[FS11], [FS12] = g.[FS12], [FS13] = g.[FS13], [FS14] = g.[FS14],
[FS15] = g.[FS15], [FS16] = g.[FS16], [FS17] = g.[FS17], [FS18] = g.[FS18], [FS19] = g.[FS19],
[FS20] = g.[FS20], [FS21] = g.[FS21], [FS22] = g.[FS22], [FS23] = g.[FS23], [FS24] = g.[FS24],
[FS25] = g.[FS25], [FS26] = g.[FS26], [FS27] = g.[FS27], [FS28] = g.[FS28], [FS29] = g.[FS29],
[FS30] = g.[FS30], [FS31] = g.[FS31], [FS32] = g.[FS32], [FS33] = g.[FS33], [FS34] = g.[FS34],
[FS35] = g.[FS35], [FS36] = g.[FS36], [FS37] = g.[FS37], [FS38] = g.[FS38], [FS39] = g.[FS39],
[FS40] = g.[FS40], [FS41] = g.[FS41], [FS42] = g.[FS42], [FS43] = g.[FS43], [FS44] = g.[FS44],
[FS45] = g.[FS45], [FS46] = g.[FS46], [FS47] = g.[FS47], [FS48] = g.[FS48], [FS49] = g.[FS49],
[FS50] = g.[FS50], [FS51] = g.[FS51], [FS52] = g.[FS52], [FS53] = g.[FS53], [FS54] = g.[FS54],
[FS55] = g.[FS55], [FS56] = g.[FS56], [FS57] = g.[FS57], [FS58] = g.[FS58], [FS59] = g.[FS59],
[FS60] = g.[FS60], [FS61] = g.[FS61], [FS62] = g.[FS62], [FS63] = g.[FS63], [FS64] = g.[FS64],
[FS65] = g.[FS65], [FS66] = g.[FS66], [FS67] = g.[FS67], [FS68] = g.[FS68], [FS69] = g.[FS69],
[FS70] = g.[FS70], [FS71] = g.[FS71], [FS72] = g.[FS72], [FS73] = g.[FS73], [FS74] = g.[FS74],
[FS75] = g.[FS75], [FS76] = g.[FS76], [FS77] = g.[FS77], [FS78] = g.[FS78], [FS79] = g.[FS79],
[FS80] = g.[FS80], [FS81] = g.[FS81], [FS82] = g.[FS82], [FS83] = g.[FS83], [FS84] = g.[FS84],
[FS85] = g.[FS85], [FS86] = g.[FS86], [FS87] = g.[FS87], [FS88] = g.[FS88], [FS89] = g.[FS89],
[FS90] = g.[FS90], [FS91] = g.[FS91], [FS92] = g.[FS92], [FS93] = g.[FS93], [FS94] = g.[FS94],
[FS95] = g.[FS95], [FS96] = g.[FS96], [FS97] = g.[FS97], [FS98] = g.[FS98], [FS99] = g.[FS99],
[MaxLevel] = g.[MaxLevel], [GuildStatus] = g.[GuildStatus]
from (select * from [dbo].[tbl_general] where [Serial]=@srcserial) as g
where [dbo].[tbl_general].[Serial]=@destserial





GO
/****** Object:  StoredProcedure [dbo].[pCopy_Inven]    Script Date: 12/01/2026 16:57:20 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO




create procedure [dbo].[pCopy_Inven]
@srcserial int,
@destserial int
as
update [dbo].[tbl_inven] set
[K0] = i.[K0], [D0] = i.[D0], [U0] = i.[U0], [K1] = i.[K1], [D1] = i.[D1], [U1] = i.[U1], [K2] = i.[K2], [D2] = i.[D2], [U2] = i.[U2], [K3] = i.[K3], [D3] =  i.[D3], [U3] = i.[U3], [K4] = i.[K4], [D4] = i.[D4], [U4] = i.[U4],
[K5] = i.[K5], [D5] = i.[D5], [U5] = i.[U5], [K6] = i.[K6], [D6] = i.[D6], [U6] = i.[U6], [K7] = i.[K7], [D7] = i.[D7], [U7] = i.[U7], [K8] = i.[K8], [D8] =  i.[D8], [U8] = i.[U8], [K9] = i.[K9], [D9] = i.[D9], [U9] = i.[U9],
[K10] = i.[K10], [D10] = i.[D10], [U10] = i.[U10], [K11] = i.[K11], [D11] = i.[D11], [U11] = i.[U11], [K12] = i.[K12], [D12] = i.[D12], [U12] = i.[U12], [K13] = i.[K13], [D13] = i.[D13], [U13] = i.[U13], [K14] = i.[K14], [D14] = i.[D14], [U14] = i.[U14],
[K15] = i.[K15], [D15] = i.[D15], [U15] = i.[U15], [K16] = i.[K16], [D16] = i.[D16], [U16] = i.[U16], [K17] = i.[K17], [D17] = i.[D17], [U17] = i.[U17], [K18] = i.[K18], [D18] = i.[D18], [U18] = i.[U18], [K19] = i.[K19], [D19] = i.[D19], [U19] = i.[U19],
[K20] = i.[K20], [D20] = i.[D20], [U20] = i.[U20], [K21] = i.[K21], [D21] = i.[D21], [U21] = i.[U21], [K22] = i.[K22], [D22] = i.[D22], [U22] = i.[U22], [K23] = i.[K23], [D23] = i.[D23], [U23] = i.[U23], [K24] = i.[K24], [D24] = i.[D24], [U24] = i.[U24],
[K25] = i.[K25], [D25] = i.[D25], [U25] = i.[U25], [K26] = i.[K26], [D26] = i.[D26], [U26] = i.[U26], [K27] = i.[K27], [D27] = i.[D27], [U27] = i.[U27], [K28] = i.[K28], [D28] = i.[D28], [U28] = i.[U28], [K29] = i.[K29], [D29] = i.[D29], [U29] = i.[U29],
[K30] = i.[K30], [D30] = i.[D30], [U30] = i.[U30], [K31] = i.[K31], [D31] = i.[D31], [U31] = i.[U31], [K32] = i.[K32], [D32] = i.[D32], [U32] = i.[U32], [K33] = i.[K33], [D33] = i.[D33], [U33] = i.[U33], [K34] = i.[K34], [D34] = i.[D34], [U34] = i.[U34],
[K35] = i.[K35], [D35] = i.[D35], [U35] = i.[U35], [K36] = i.[K36], [D36] = i.[D36], [U36] = i.[U36], [K37] = i.[K37], [D37] = i.[D37], [U37] = i.[U37], [K38] = i.[K38], [D38] = i.[D38], [U38] = i.[U38], [K39] = i.[K39], [D39] = i.[D39], [U39] = i.[U39],
[K40] = i.[K40], [D40] = i.[D40], [U40] = i.[U40], [K41] = i.[K41], [D41] = i.[D41], [U41] = i.[U41], [K42] = i.[K42], [D42] = i.[D42], [U42] = i.[U42], [K43] = i.[K43], [D43] = i.[D43], [U43] = i.[U43], [K44] = i.[K44], [D44] = i.[D44], [U44] = i.[U44],
[K45] = i.[K45], [D45] = i.[D45], [U45] = i.[U45], [K46] = i.[K46], [D46] = i.[D46], [U46] = i.[U46], [K47] = i.[K47], [D47] = i.[D47], [U47] = i.[U47], [K48] = i.[K48], [D48] = i.[D48], [U48] = i.[U48], [K49] = i.[K49], [D49] = i.[D49], [U49] = i.[U49],
[K50] = i.[K50], [D50] = i.[D50], [U50] = i.[U50], [K51] = i.[K51], [D51] = i.[D51], [U51] = i.[U51], [K52] = i.[K52], [D52] = i.[D52], [U52] = i.[U52], [K53] = i.[K53], [D53] = i.[D53], [U53] = i.[U53], [K54] = i.[K54], [D54] = i.[D54], [U54] = i.[U54],
[K55] = i.[K55], [D55] = i.[D55], [U55] = i.[U55], [K56] = i.[K56], [D56] = i.[D56], [U56] = i.[U56], [K57] = i.[K57], [D57] = i.[D57], [U57] = i.[U57], [K58] = i.[K58], [D58] = i.[D58], [U58] = i.[U58], [K59] = i.[K59], [D59] = i.[D59], [U59] = i.[U59],
[K60] = i.[K60], [D60] = i.[D60], [U60] = i.[U60], [K61] = i.[K61], [D61] = i.[D61], [U61] = i.[U61], [K62] = i.[K62], [D62] = i.[D62], [U62] = i.[U62], [K63] = i.[K63], [D63] = i.[D63], [U63] = i.[U63], [K64] = i.[K64], [D64] = i.[D64], [U64] = i.[U64],
[K65] = i.[K65], [D65] = i.[D65], [U65] = i.[U65], [K66] = i.[K66], [D66] = i.[D66], [U66] = i.[U66], [K67] = i.[K67], [D67] = i.[D67], [U67] = i.[U67], [K68] = i.[K68], [D68] = i.[D68], [U68] = i.[U68], [K69] = i.[K69], [D69] = i.[D69], [U69] = i.[U69],
[K70] = i.[K70], [D70] = i.[D70], [U70] = i.[U70], [K71] = i.[K71], [D71] = i.[D71], [U71] = i.[U71], [K72] = i.[K72], [D72] = i.[D72], [U72] = i.[U72], [K73] = i.[K73], [D73] = i.[D73], [U73] = i.[U73], [K74] = i.[K74], [D74] = i.[D74], [U74] = i.[U74],
[K75] = i.[K75], [D75] = i.[D75], [U75] = i.[U75], [K76] = i.[K76], [D76] = i.[D76], [U76] = i.[U76], [K77] = i.[K77], [D77] = i.[D77], [U77] = i.[U77], [K78] = i.[K78], [D78] = i.[D78], [U78] = i.[U78], [K79] = i.[K79], [D79] = i.[D79], [U79] = i.[U79],
[K80] = i.[K80], [D80] = i.[D80], [U80] = i.[U80], [K81] = i.[K81], [D81] = i.[D81], [U81] = i.[U81], [K82] = i.[K82], [D82] = i.[D82], [U82] = i.[U82], [K83] = i.[K83], [D83] = i.[D83], [U83] = i.[U83], [K84] = i.[K84], [D84] = i.[D84], [U84] = i.[U84],
[K85] = i.[K85], [D85] = i.[D85], [U85] = i.[U85], [K86] = i.[K86], [D86] = i.[D86], [U86] = i.[U86], [K87] = i.[K87], [D87] = i.[D87], [U87] = i.[U87], [K88] = i.[K88], [D88] = i.[D88], [U88] = i.[U88], [K89] = i.[K89], [D89] = i.[D89], [U89] = i.[U89],
[K90] = i.[K90], [D90] = i.[D90], [U90] = i.[U90], [K91] = i.[K91], [D91] = i.[D91], [U91] = i.[U91], [K92] = i.[K92], [D92] = i.[D92], [U92] = i.[U92], [K93] = i.[K93], [D93] = i.[D93], [U93] = i.[U93], [K94] = i.[K94], [D94] = i.[D94], [U94] = i.[U94],
[K95] = i.[K95], [D95] = i.[D95], [U95] = i.[U95], [K96] = i.[K96], [D96] = i.[D96], [U96] = i.[U96], [K97] = i.[K97], [D97] = i.[D97], [U97] = i.[U97], [K98] = i.[K98], [D98] = i.[D98], [U98] = i.[U98], [K99] = i.[K99], [D99] = i.[D99], [U99] = i.[U99],
[S0] = i.[S0], [T0] = i.[T0], [S1] = i.[S1], [T1] = i.[T1], [S2] = i.[S2], [T2] = i.[T2], [S3] = i.[S3], [T3] = i.[T3], [S4] = i.[S4], [T4] = i.[T4],
[S5] = i.[S5], [T5] = i.[T5], [S6] = i.[S6], [T6] = i.[T6], [S7] = i.[S7], [T7] = i.[T7], [S8] = i.[S8], [T8] = i.[T8], [S9] = i.[S9], [T9] = i.[T9],
[S10] = i.[S10], [T10] = i.[T10], [S11] = i.[S11], [T11] = i.[T11], [S12] = i.[S12], [T12] = i.[T12], [S13] = i.[S13], [T13] = i.[T13], [S14] = i.[S14], [T14] = i.[T14],
[S15] = i.[S15], [T15] = i.[T15], [S16] = i.[S16], [T16] = i.[T16], [S17] = i.[S17], [T17] = i.[T17], [S18] = i.[S18], [T18] = i.[T18], [S19] = i.[S19], [T19] = i.[T19],
[S20] = i.[S20], [T20] = i.[T20], [S21] = i.[S21], [T21] = i.[T21], [S22] = i.[S22], [T22] = i.[T22], [S23] = i.[S23], [T23] = i.[T23], [S24] = i.[S24], [T24] = i.[T24],
[S25] = i.[S25], [T25] = i.[T25], [S26] = i.[S26], [T26] = i.[T26], [S27] = i.[S27], [T27] = i.[T27], [S28] = i.[S28], [T28] = i.[T28], [S29] = i.[S29], [T29] = i.[T29],
[S30] = i.[S30], [T30] = i.[T30], [S31] = i.[S31], [T31] = i.[T31], [S32] = i.[S32], [T32] = i.[T32], [S33] = i.[S33], [T33] = i.[T33], [S34] = i.[S34], [T34] = i.[T34],
[S35] = i.[S35], [T35] = i.[T35], [S36] = i.[S36], [T36] = i.[T36], [S37] = i.[S37], [T37] = i.[T37], [S38] = i.[S38], [T38] = i.[T38], [S39] = i.[S39], [T39] = i.[T39],
[S40] = i.[S40], [T40] = i.[T40], [S41] = i.[S41], [T41] = i.[T41], [S42] = i.[S42], [T42] = i.[T42], [S43] = i.[S43], [T43] = i.[T43], [S44] = i.[S44], [T44] = i.[T44],
[S45] = i.[S45], [T45] = i.[T45], [S46] = i.[S46], [T46] = i.[T46], [S47] = i.[S47], [T47] = i.[T47], [S48] = i.[S48], [T48] = i.[T48], [S49] = i.[S49], [T49] = i.[T49], 
[S50] = i.[S50], [T50] = i.[T50], [S51] = i.[S51], [T51] = i.[T51], [S52] = i.[S52], [T52] = i.[T52], [S53] = i.[S53], [T53] = i.[T53], [S54] = i.[S54], [T54] = i.[T54],
[S55] = i.[S55], [T55] = i.[T55], [S56] = i.[S56], [T56] = i.[T56], [S57] = i.[S57], [T57] = i.[T57], [S58] = i.[S58], [T58] = i.[T58], [S59] = i.[S59], [T59] = i.[T59],
[S60] = i.[S60], [T60] = i.[T60], [S61] = i.[S61], [T61] = i.[T61], [S62] = i.[S62], [T62] = i.[T62], [S63] = i.[S63], [T63] = i.[T63], [S64] = i.[S64], [T64] = i.[T64],
[S65] = i.[S65], [T65] = i.[T65], [S66] = i.[S66], [T66] = i.[T66], [S67] = i.[S67], [T67] = i.[T67], [S68] = i.[S68], [T68] = i.[T68], [S69] = i.[S69], [T69] = i.[T69],
[S70] = i.[S70], [T70] = i.[T70], [S71] = i.[S71], [T71] = i.[T71], [S72] = i.[S72], [T72] = i.[T72], [S73] = i.[S73], [T73] = i.[T73], [S74] = i.[S74], [T74] = i.[T74],
[S75] = i.[S75], [T75] = i.[T75], [S76] = i.[S76], [T76] = i.[T76], [S77] = i.[S77], [T77] = i.[T77], [S78] = i.[S78], [T78] = i.[T78], [S79] = i.[S79], [T79] = i.[T79],
[S80] = i.[S80], [T80] = i.[T80], [S81] = i.[S81], [T81] = i.[T81], [S82] = i.[S82], [T82] = i.[T82], [S83] = i.[S83], [T83] = i.[T83], [S84] = i.[S84], [T84] = i.[T84],
[S85] = i.[S85], [T85] = i.[T85], [S86] = i.[S86], [T86] = i.[T86], [S87] = i.[S87], [T87] = i.[T87], [S88] = i.[S88], [T88] = i.[T88], [S89] = i.[S89], [T89] = i.[T89],
[S90] = i.[S90], [T90] = i.[T90], [S91] = i.[S91], [T91] = i.[T91], [S92] = i.[S92], [T92] = i.[T92], [S93] = i.[S93], [T93] = i.[T93], [S94] = i.[S94], [T94] = i.[T94],
[S95] = i.[S95], [T95] = i.[T95], [S96] = i.[S96], [T96] = i.[T96], [S97] = i.[S97], [T97] = i.[T97], [S98] = i.[S98], [T98] = i.[T98], [S99] = i.[S99], [T99] = i.[T99]
from (select * from [dbo].[tbl_inven] where [Serial]=@srcserial) as i
where [dbo].[tbl_inven].[Serial] = @destserial




GO
/****** Object:  StoredProcedure [dbo].[pCopy_MakeAvator]    Script Date: 12/01/2026 16:57:20 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO




create procedure [dbo].[pCopy_MakeAvator]
@srcserial int,
@destserial int
as
exec [dbo].[pCopy_Base] @srcserial, @destserial
exec [dbo].[pCopy_General] @srcserial, @destserial
exec [dbo].[pCopy_Inven] @srcserial, @destserial
exec [dbo].[pCopy_Unit] @srcserial, @destserial
exec [dbo].[pCopy_Quest] @srcserial, @destserial
exec [dbo].[pCopy_NpcQuestHistory] @srcserial, @destserial
exec [dbo].[pCopy_Supplement] @srcserial, @destserial




GO
/****** Object:  StoredProcedure [dbo].[pCopy_NpcQuestHistory]    Script Date: 12/01/2026 16:57:20 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO




-- declare @i int
-- declare @j int
-- declare @str varchar(1024)
-- set @i = 0
-- set @j = 0
-- set @str = ''
-- while @i < 30
-- begin
-- 	set @j = 0
-- 	set @str = ''
-- 
-- 	set @str = @str + '[Type' + convert(varchar, @i) + '] = q.[Type' + convert(varchar, @i) +'], '
-- 	+ '[QuestIndex' + convert(varchar, @i) +'] = q.[QuestIndex' + convert(varchar, @i) +'], '
-- 	+ '[FirstAct' + convert(varchar, @i) + '] = q.[FirstAct' + convert(varchar, @i) + '], '
-- 	+ '[SecondAct' + convert(varchar, @i) + '] = q.[SecondAct' + convert(varchar, @i) + '], '
-- 	+ '[ThirdAct' + convert(varchar, @i) + '] = q.[ThirdAct' + convert(varchar, @i) + '], '
-- 	+ '[PassSec' + convert(varchar, @i) + '] = q.[PassSec' + convert(varchar, @i) + '],'
-- 	set @j = @j + 1
-- 	set @i = @i + 1
-- 
-- 	print @str
-- end


create procedure [dbo].[pCopy_NpcQuestHistory]
@srcserial int,
@destserial int
as
update [dbo].[tbl_npc_quest_history] set
[Code1] = nqh.[Code1], [Level1] = nqh.[Level1], [Code2] = nqh.[Code2], [Level2] = nqh.[Level2],
[Code3] = nqh.[Code3], [Level3] = nqh.[Level3], [Code4] = nqh.[Code4], [Level4] = nqh.[Level4],
[Code5] = nqh.[Code5], [Level5] = nqh.[Level5], [Code6] = nqh.[Code6], [Level6] = nqh.[Level6],
[Code7] = nqh.[Code7], [Level7] = nqh.[Level7], [Code8] = nqh.[Code8], [Level8] = nqh.[Level8],
[Code9] = nqh.[Code9], [Level9] = nqh.[Level9], [Code10] = nqh.[Code10], [Level10] = nqh.[Level10],
[Code11] = nqh.[Code11], [Level11] = nqh.[Level11], [Code12] = nqh.[Code12], [Level12] = nqh.[Level12],
[Code13] = nqh.[Code13], [Level13] = nqh.[Level13], [Code14] = nqh.[Code14], [Level14] = nqh.[Level14],
[Code15] = nqh.[Code15], [Level15] = nqh.[Level15], [Code16] = nqh.[Code16], [Level16] = nqh.[Level16],
[Code17] = nqh.[Code17], [Level17] = nqh.[Level17], [Code18] = nqh.[Code18], [Level18] = nqh.[Level18], 
[Code19] = nqh.[Code19], [Level19] = nqh.[Level19], [Code20] = nqh.[Code20], [Level20] = nqh.[Level20]
from (select * from [dbo].[tbl_npc_quest_history] where [Serial] = @srcserial ) as nqh
where [dbo].[tbl_npc_quest_history].[Serial] = @destserial





GO
/****** Object:  StoredProcedure [dbo].[pCopy_Quest]    Script Date: 12/01/2026 16:57:20 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO




create procedure [dbo].[pCopy_Quest]
@srcserial int,
@destserial int
as
update [dbo].[tbl_quest] set
[Type0] = q.[Type0], [QuestIndex0] = q.[QuestIndex0], [FirstAct0] = q.[FirstAct0], [SecondAct0] = q.[SecondAct0], [ThirdAct0] = q.[ThirdAct0], [PassSec0] = q.[PassSec0],
[Type1] = q.[Type1], [QuestIndex1] = q.[QuestIndex1], [FirstAct1] = q.[FirstAct1], [SecondAct1] = q.[SecondAct1], [ThirdAct1] = q.[ThirdAct1], [PassSec1] = q.[PassSec1],
[Type2] = q.[Type2], [QuestIndex2] = q.[QuestIndex2], [FirstAct2] = q.[FirstAct2], [SecondAct2] = q.[SecondAct2], [ThirdAct2] = q.[ThirdAct2], [PassSec2] = q.[PassSec2],
[Type3] = q.[Type3], [QuestIndex3] = q.[QuestIndex3], [FirstAct3] = q.[FirstAct3], [SecondAct3] = q.[SecondAct3], [ThirdAct3] = q.[ThirdAct3], [PassSec3] = q.[PassSec3],
[Type4] = q.[Type4], [QuestIndex4] = q.[QuestIndex4], [FirstAct4] = q.[FirstAct4], [SecondAct4] = q.[SecondAct4], [ThirdAct4] = q.[ThirdAct4], [PassSec4] = q.[PassSec4],
[Type5] = q.[Type5], [QuestIndex5] = q.[QuestIndex5], [FirstAct5] = q.[FirstAct5], [SecondAct5] = q.[SecondAct5], [ThirdAct5] = q.[ThirdAct5], [PassSec5] = q.[PassSec5],
[Type6] = q.[Type6], [QuestIndex6] = q.[QuestIndex6], [FirstAct6] = q.[FirstAct6], [SecondAct6] = q.[SecondAct6], [ThirdAct6] = q.[ThirdAct6], [PassSec6] = q.[PassSec6],
[Type7] = q.[Type7], [QuestIndex7] = q.[QuestIndex7], [FirstAct7] = q.[FirstAct7], [SecondAct7] = q.[SecondAct7], [ThirdAct7] = q.[ThirdAct7], [PassSec7] = q.[PassSec7],
[Type8] = q.[Type8], [QuestIndex8] = q.[QuestIndex8], [FirstAct8] = q.[FirstAct8], [SecondAct8] = q.[SecondAct8], [ThirdAct8] = q.[ThirdAct8], [PassSec8] = q.[PassSec8],
[Type9] = q.[Type9], [QuestIndex9] = q.[QuestIndex9], [FirstAct9] = q.[FirstAct9], [SecondAct9] = q.[SecondAct9], [ThirdAct9] = q.[ThirdAct9], [PassSec9] = q.[PassSec9],
[Type10] = q.[Type10], [QuestIndex10] = q.[QuestIndex10], [FirstAct10] = q.[FirstAct10], [SecondAct10] = q.[SecondAct10], [ThirdAct10] = q.[ThirdAct10], [PassSec10] = q.[PassSec10],
[Type11] = q.[Type11], [QuestIndex11] = q.[QuestIndex11], [FirstAct11] = q.[FirstAct11], [SecondAct11] = q.[SecondAct11], [ThirdAct11] = q.[ThirdAct11], [PassSec11] = q.[PassSec11],
[Type12] = q.[Type12], [QuestIndex12] = q.[QuestIndex12], [FirstAct12] = q.[FirstAct12], [SecondAct12] = q.[SecondAct12], [ThirdAct12] = q.[ThirdAct12], [PassSec12] = q.[PassSec12],
[Type13] = q.[Type13], [QuestIndex13] = q.[QuestIndex13], [FirstAct13] = q.[FirstAct13], [SecondAct13] = q.[SecondAct13], [ThirdAct13] = q.[ThirdAct13], [PassSec13] = q.[PassSec13],
[Type14] = q.[Type14], [QuestIndex14] = q.[QuestIndex14], [FirstAct14] = q.[FirstAct14], [SecondAct14] = q.[SecondAct14], [ThirdAct14] = q.[ThirdAct14], [PassSec14] = q.[PassSec14],
[Type15] = q.[Type15], [QuestIndex15] = q.[QuestIndex15], [FirstAct15] = q.[FirstAct15], [SecondAct15] = q.[SecondAct15], [ThirdAct15] = q.[ThirdAct15], [PassSec15] = q.[PassSec15],
[Type16] = q.[Type16], [QuestIndex16] = q.[QuestIndex16], [FirstAct16] = q.[FirstAct16], [SecondAct16] = q.[SecondAct16], [ThirdAct16] = q.[ThirdAct16], [PassSec16] = q.[PassSec16],
[Type17] = q.[Type17], [QuestIndex17] = q.[QuestIndex17], [FirstAct17] = q.[FirstAct17], [SecondAct17] = q.[SecondAct17], [ThirdAct17] = q.[ThirdAct17], [PassSec17] = q.[PassSec17],
[Type18] = q.[Type18], [QuestIndex18] = q.[QuestIndex18], [FirstAct18] = q.[FirstAct18], [SecondAct18] = q.[SecondAct18], [ThirdAct18] = q.[ThirdAct18], [PassSec18] = q.[PassSec18],
[Type19] = q.[Type19], [QuestIndex19] = q.[QuestIndex19], [FirstAct19] = q.[FirstAct19], [SecondAct19] = q.[SecondAct19], [ThirdAct19] = q.[ThirdAct19], [PassSec19] = q.[PassSec19],
[Type20] = q.[Type20], [QuestIndex20] = q.[QuestIndex20], [FirstAct20] = q.[FirstAct20], [SecondAct20] = q.[SecondAct20], [ThirdAct20] = q.[ThirdAct20], [PassSec20] = q.[PassSec20],
[Type21] = q.[Type21], [QuestIndex21] = q.[QuestIndex21], [FirstAct21] = q.[FirstAct21], [SecondAct21] = q.[SecondAct21], [ThirdAct21] = q.[ThirdAct21], [PassSec21] = q.[PassSec21],
[Type22] = q.[Type22], [QuestIndex22] = q.[QuestIndex22], [FirstAct22] = q.[FirstAct22], [SecondAct22] = q.[SecondAct22], [ThirdAct22] = q.[ThirdAct22], [PassSec22] = q.[PassSec22],
[Type23] = q.[Type23], [QuestIndex23] = q.[QuestIndex23], [FirstAct23] = q.[FirstAct23], [SecondAct23] = q.[SecondAct23], [ThirdAct23] = q.[ThirdAct23], [PassSec23] = q.[PassSec23],
[Type24] = q.[Type24], [QuestIndex24] = q.[QuestIndex24], [FirstAct24] = q.[FirstAct24], [SecondAct24] = q.[SecondAct24], [ThirdAct24] = q.[ThirdAct24], [PassSec24] = q.[PassSec24],
[Type25] = q.[Type25], [QuestIndex25] = q.[QuestIndex25], [FirstAct25] = q.[FirstAct25], [SecondAct25] = q.[SecondAct25], [ThirdAct25] = q.[ThirdAct25], [PassSec25] = q.[PassSec25],
[Type26] = q.[Type26], [QuestIndex26] = q.[QuestIndex26], [FirstAct26] = q.[FirstAct26], [SecondAct26] = q.[SecondAct26], [ThirdAct26] = q.[ThirdAct26], [PassSec26] = q.[PassSec26],
[Type27] = q.[Type27], [QuestIndex27] = q.[QuestIndex27], [FirstAct27] = q.[FirstAct27], [SecondAct27] = q.[SecondAct27], [ThirdAct27] = q.[ThirdAct27], [PassSec27] = q.[PassSec27],
[Type28] = q.[Type28], [QuestIndex28] = q.[QuestIndex28], [FirstAct28] = q.[FirstAct28], [SecondAct28] = q.[SecondAct28], [ThirdAct28] = q.[ThirdAct28], [PassSec28] = q.[PassSec28],
[Type29] = q.[Type29], [QuestIndex29] = q.[QuestIndex29], [FirstAct29] = q.[FirstAct29], [SecondAct29] = q.[SecondAct29], [ThirdAct29] = q.[ThirdAct29], [PassSec29] = q.[PassSec29]
from (select * from [dbo].[tbl_quest] where [Serial] = @srcserial ) as q
where [dbo].[tbl_quest].[Serial] = @destserial





GO
/****** Object:  StoredProcedure [dbo].[pCopy_Supplement]    Script Date: 12/01/2026 16:57:20 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO



-- declare @i int
-- declare @j int
-- declare @str varchar(1024)
-- set @i = 1
-- set @j = 0
-- set @str = ''
-- while @i < 20
-- begin
-- 	set @j = 0
-- 	set @str = ''
-- 
-- 	while @j < 2
-- 	begin
-- 		set @str = @str + '[Code' + convert(varchar, @i) + '] = nqh.[Code' + convert(varchar, @i) +'], [Level' + convert(varchar, @i) +'] = nqh.[Level' + convert(varchar, @i) +'], '
-- 		if @i = 99 break
-- 		set @j = @j + 1
-- 		set @i = @i + 1
-- 	end
-- 	print @str
-- end


create procedure [dbo].[pCopy_Supplement]
@srcserial int,
@destserial int
as
update [dbo].[tbl_supplement] set
[PvpPointLeak] = sm.[PvpPointLeak],
[LastAttBuff] = sm.[LastAttBuff],
[cristalbattledate] = sm.[cristalbattledate]
from (select * from [dbo].[tbl_supplement] where [Serial] = @srcserial )as sm
where [dbo].[tbl_supplement].[Serial] = @destserial





GO
/****** Object:  StoredProcedure [dbo].[pCopy_UI]    Script Date: 12/01/2026 16:57:20 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO





--
CREATE PROCEDURE [dbo].[pCopy_UI]
@nSrcSerial int,
@nDstSerial int
AS
UPDATE
[dbo].[tbl_UserInterface]
SET

LB0 = src.LB0, LB1 = src.LB1, LB2 = src.LB2, LB3 = src.LB3, LB4 = src.LB4, 
LB5 = src.LB5, LB6 = src.LB6, LB7 = src.LB7, LB8 = src.LB8, LB9 = src.LB9,
LB10 = src.LB10, LB11 = src.LB11, LB12 = src.LB12, LB13 = src.LB13, LB14 = src.LB14, 
LB15 = src.LB15, LB16 = src.LB16, LB17 = src.LB17, LB18 = src.LB18, LB19 = src.LB19,
LB20 = src.LB20, LB21 = src.LB21, LB22 = src.LB22, LB23 = src.LB23, LB24 = src.LB24, 
LB25 = src.LB25, LB26 = src.LB26, LB27 = src.LB27, LB28 = src.LB28, LB29 = src.LB29,
LB30 = src.LB30, LB31 = src.LB31, LB32 = src.LB32, LB33 = src.LB33, LB34 = src.LB34, 
LB35 = src.LB35, LB36 = src.LB36, LB37 = src.LB37, LB38 = src.LB38, LB39 = src.LB39,
LB40 = src.LB40, LB41 = src.LB41, LB42 = src.LB42, LB43 = src.LB43, LB44 = src.LB44, 
LB45 = src.LB45, LB46 = src.LB46, LB47 = src.LB47, LB48 = src.LB48, LB49 = src.LB49,
DCF0 = src.DCF0, DCF1 = src.DCF1, DCF2 = src.DCF2, DCF3 = src.DCF3, DCF4 = src.DCF4, 
HCF0 = src.HCF0, HCF1 = src.HCF1, HCF2 = src.HCF2, HCF3 = src.HCF3, HCF4 = src.HCF4

from(select * from [dbo].[tbl_UserInterface] where serial=@nSrcSerial) as src where [dbo].[tbl_UserInterface].serial=@nDstSerial






GO
/****** Object:  StoredProcedure [dbo].[pCopy_Unit]    Script Date: 12/01/2026 16:57:20 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO


CREATE procedure [dbo].[pCopy_Unit]
@srcserial int,
@destserial int
as
update [dbo].[tbl_unit] set
[F_0] = u.[F_0], [H_0] = u.[H_0], [U_0] = u.[U_0], [L_0] = u.[L_0], [A_0] = u.[A_0], [S_0] = u.[S_0], [B_0] = u.[B_0], [Gg_0] = u.[Gg_0], [AB_0] = u.[AB_0], [SB_0] = u.[SB_0],
[Sp0_0] = u.[Sp0_0], [Sp1_0] = u.[Sp1_0], [Sp2_0] = u.[Sp2_0], [Sp3_0] = u.[Sp3_0], [Sp4_0] = u.[Sp4_0], [Sp5_0] = u.[Sp5_0], [Sp6_0] = u.[Sp6_0], [Sp7_0] = u.[Sp7_0],
[KF_0] = u.[KF_0], [PF_0] = u.[PF_0], [Cut_0] = u.[Cut_0],
[F_1] = u.[F_1], [H_1] = u.[H_1], [U_1] = u.[U_1], [L_1] = u.[L_1], [A_1] = u.[A_1], [S_1] = u.[S_1], [B_1] = u.[B_1], [Gg_1] = u.[Gg_1], [AB_1] = u.[AB_1], [SB_1] = u.[SB_1],
[Sp0_1] = u.[Sp0_1], [Sp1_1] = u.[Sp1_1], [Sp2_1] = u.[Sp2_1], [Sp3_1] = u.[Sp3_1], [Sp4_1] = u.[Sp4_1], [Sp5_1] = u.[Sp5_1], [Sp6_1] = u.[Sp6_1], [Sp7_1] = u.[Sp7_1],
[KF_1] = u.[KF_1], [PF_1] = u.[PF_1], [Cut_1] = u.[Cut_1],
[F_2] = u.[F_2], [H_2] = u.[H_2], [U_2] = u.[U_2], [L_2] = u.[L_2], [A_2] = u.[A_2], [S_2] = u.[S_2], [B_2] = u.[B_2], [Gg_2] = u.[Gg_2], [AB_2] = u.[AB_2], [SB_2] = u.[SB_2],
[Sp0_2] = u.[Sp0_2], [Sp1_2] = u.[Sp1_2], [Sp2_2] = u.[Sp2_2], [Sp3_2] = u.[Sp3_2], [Sp4_2] = u.[Sp4_2], [Sp5_2] = u.[Sp5_2], [Sp6_2] = u.[Sp6_2], [Sp7_2] = u.[Sp7_2],
[KF_2] = u.[KF_2], [PF_2] = u.[PF_2], [Cut_2] = u.[Cut_2],
[F_3] = u.[F_3], [H_3] = u.[H_3], [U_3] = u.[U_3], [L_3] = u.[L_3], [A_3] = u.[A_3], [S_3] = u.[S_3], [B_3] = u.[B_3], [Gg_3] = u.[Gg_3], [AB_3] = u.[AB_3], [SB_3] = u.[SB_3],
[Sp0_3] = u.[Sp0_3], [Sp1_3] = u.[Sp1_3], [Sp2_3] = u.[Sp2_3], [Sp3_3] = u.[Sp3_3], [Sp4_3] = u.[Sp4_3], [Sp5_3] = u.[Sp5_3], [Sp6_3] = u.[Sp6_3], [Sp7_3] = u.[Sp7_3],
[KF_3] = u.[KF_3], [PF_3] = u.[PF_3], [Cut_3] = u.[Cut_3]
from ( select * from [dbo].[tbl_unit] where [Serial] = @srcserial ) as u
where [dbo].[tbl_unit].[Serial] = @destserial





GO
/****** Object:  StoredProcedure [dbo].[pcreate_aminepersonal_inven]    Script Date: 12/01/2026 16:57:20 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO






create procedure [dbo].[pcreate_aminepersonal_inven]
as
create table [dbo].[tbl_aminepersonal_inven]
(
	[serial] int identity(1, 1) not null,
	[avatorserial] [int] not null,
	[dck] [bit] not null,
	[tmReg] [datetime] not null,
	[tmUpdate] [datetime] not null,

	[K0] [int] not null, [N0] [tinyint] not null, [K1] [int] not null, [N1] [tinyint] not null, 
	[K2] [int] not null, [N2] [tinyint] not null, [K3] [int] not null, [N3] [tinyint] not null, 
	[K4] [int] not null, [N4] [tinyint] not null, [K5] [int] not null, [N5] [tinyint] not null, 
	[K6] [int] not null, [N6] [tinyint] not null, [K7] [int] not null, [N7] [tinyint] not null, 
	[K8] [int] not null, [N8] [tinyint] not null, [K9] [int] not null, [N9] [tinyint] not null, 

	[K10] [int] not null, [N10] [tinyint] not null, [K11] [int] not null, [N11] [tinyint] not null, 
	[K12] [int] not null, [N12] [tinyint] not null, [K13] [int] not null, [N13] [tinyint] not null, 
	[K14] [int] not null, [N14] [tinyint] not null, [K15] [int] not null, [N15] [tinyint] not null, 
	[K16] [int] not null, [N16] [tinyint] not null, [K17] [int] not null, [N17] [tinyint] not null, 
	[K18] [int] not null, [N18] [tinyint] not null, [K19] [int] not null, [N19] [tinyint] not null, 

	[K20] [int] not null, [N20] [tinyint] not null, [K21] [int] not null, [N21] [tinyint] not null, 
	[K22] [int] not null, [N22] [tinyint] not null, [K23] [int] not null, [N23] [tinyint] not null, 
	[K24] [int] not null, [N24] [tinyint] not null, [K25] [int] not null, [N25] [tinyint] not null, 
	[K26] [int] not null, [N26] [tinyint] not null, [K27] [int] not null, [N27] [tinyint] not null, 
	[K28] [int] not null, [N28] [tinyint] not null, [K29] [int] not null, [N29] [tinyint] not null, 

	[K30] [int] not null, [N30] [tinyint] not null, [K31] [int] not null, [N31] [tinyint] not null, 
	[K32] [int] not null, [N32] [tinyint] not null, [K33] [int] not null, [N33] [tinyint] not null, 
	[K34] [int] not null, [N34] [tinyint] not null, [K35] [int] not null, [N35] [tinyint] not null, 
	[K36] [int] not null, [N36] [tinyint] not null, [K37] [int] not null, [N37] [tinyint] not null, 
	[K38] [int] not null, [N38] [tinyint] not null, [K39] [int] not null, [N39] [tinyint] not null
)
	
alter table [dbo].[tbl_aminepersonal_inven] with nocheck add
	constraint [PK_tbl_aminepersonal_inven] primary key clustered
	([serial],[avatorserial]) on [primary]

alter table [dbo].[tbl_aminepersonal_inven] with nocheck add
	constraint [DF_tbl_aminepersonal_inven_avatorserial] default (0xFFFFFFFF) for [avatorserial],
	constraint [DF_tbl_aminepersonal_inven_dck] default (0) for [dck],
	constraint [DF_tbl_aminepersonal_inven_tmReg] default (getdate()) for [tmReg],
	constraint [DF_tbl_aminepersonal_inven_tmUpdate] default (getdate()) for [tmUpdate],

	constraint [DF_tbl_aminepersonal_K0] default (0xFFFFFFFF) for [K0], constraint [DF_tbl_aminepersonal_inven_N0] default (0) for [N0],
	constraint [DF_tbl_aminepersonal_K1] default (0xFFFFFFFF) for [K1], constraint [DF_tbl_aminepersonal_inven_N1] default (0) for [N1],
	constraint [DF_tbl_aminepersonal_K2] default (0xFFFFFFFF) for [K2], constraint [DF_tbl_aminepersonal_inven_N2] default (0) for [N2],
	constraint [DF_tbl_aminepersonal_K3] default (0xFFFFFFFF) for [K3], constraint [DF_tbl_aminepersonal_inven_N3] default (0) for [N3],
	constraint [DF_tbl_aminepersonal_K4] default (0xFFFFFFFF) for [K4], constraint [DF_tbl_aminepersonal_inven_N4] default (0) for [N4],
	constraint [DF_tbl_aminepersonal_K5] default (0xFFFFFFFF) for [K5], constraint [DF_tbl_aminepersonal_inven_N5] default (0) for [N5],
	constraint [DF_tbl_aminepersonal_K6] default (0xFFFFFFFF) for [K6], constraint [DF_tbl_aminepersonal_inven_N6] default (0) for [N6],
	constraint [DF_tbl_aminepersonal_K7] default (0xFFFFFFFF) for [K7], constraint [DF_tbl_aminepersonal_inven_N7] default (0) for [N7],
	constraint [DF_tbl_aminepersonal_K8] default (0xFFFFFFFF) for [K8], constraint [DF_tbl_aminepersonal_inven_N8] default (0) for [N8],
	constraint [DF_tbl_aminepersonal_K9] default (0xFFFFFFFF) for [K9], constraint [DF_tbl_aminepersonal_inven_N9] default (0) for [N9],
	
	constraint [DF_tbl_aminepersonal_K10] default (0xFFFFFFFF) for [K10], constraint [DF_tbl_aminepersonal_inven_N10] default (0) for [N10],
	constraint [DF_tbl_aminepersonal_K11] default (0xFFFFFFFF) for [K11], constraint [DF_tbl_aminepersonal_inven_N11] default (0) for [N11],
	constraint [DF_tbl_aminepersonal_K12] default (0xFFFFFFFF) for [K12], constraint [DF_tbl_aminepersonal_inven_N12] default (0) for [N12],
	constraint [DF_tbl_aminepersonal_K13] default (0xFFFFFFFF) for [K13], constraint [DF_tbl_aminepersonal_inven_N13] default (0) for [N13],
	constraint [DF_tbl_aminepersonal_K14] default (0xFFFFFFFF) for [K14], constraint [DF_tbl_aminepersonal_inven_N14] default (0) for [N14],
	constraint [DF_tbl_aminepersonal_K15] default (0xFFFFFFFF) for [K15], constraint [DF_tbl_aminepersonal_inven_N15] default (0) for [N15],
	constraint [DF_tbl_aminepersonal_K16] default (0xFFFFFFFF) for [K16], constraint [DF_tbl_aminepersonal_inven_N16] default (0) for [N16],
	constraint [DF_tbl_aminepersonal_K17] default (0xFFFFFFFF) for [K17], constraint [DF_tbl_aminepersonal_inven_N17] default (0) for [N17],
	constraint [DF_tbl_aminepersonal_K18] default (0xFFFFFFFF) for [K18], constraint [DF_tbl_aminepersonal_inven_N18] default (0) for [N18],
	constraint [DF_tbl_aminepersonal_K19] default (0xFFFFFFFF) for [K19], constraint [DF_tbl_aminepersonal_inven_N19] default (0) for [N19],
	
	constraint [DF_tbl_aminepersonal_K20] default (0xFFFFFFFF) for [K20], constraint [DF_tbl_aminepersonal_inven_N20] default (0) for [N20],
	constraint [DF_tbl_aminepersonal_K21] default (0xFFFFFFFF) for [K21], constraint [DF_tbl_aminepersonal_inven_N21] default (0) for [N21],
	constraint [DF_tbl_aminepersonal_K22] default (0xFFFFFFFF) for [K22], constraint [DF_tbl_aminepersonal_inven_N22] default (0) for [N22],
	constraint [DF_tbl_aminepersonal_K23] default (0xFFFFFFFF) for [K23], constraint [DF_tbl_aminepersonal_inven_N23] default (0) for [N23],
	constraint [DF_tbl_aminepersonal_K24] default (0xFFFFFFFF) for [K24], constraint [DF_tbl_aminepersonal_inven_N24] default (0) for [N24],
	constraint [DF_tbl_aminepersonal_K25] default (0xFFFFFFFF) for [K25], constraint [DF_tbl_aminepersonal_inven_N25] default (0) for [N25],
	constraint [DF_tbl_aminepersonal_K26] default (0xFFFFFFFF) for [K26], constraint [DF_tbl_aminepersonal_inven_N26] default (0) for [N26],
	constraint [DF_tbl_aminepersonal_K27] default (0xFFFFFFFF) for [K27], constraint [DF_tbl_aminepersonal_inven_N27] default (0) for [N27],
	constraint [DF_tbl_aminepersonal_K28] default (0xFFFFFFFF) for [K28], constraint [DF_tbl_aminepersonal_inven_N28] default (0) for [N28],
	constraint [DF_tbl_aminepersonal_K29] default (0xFFFFFFFF) for [K29], constraint [DF_tbl_aminepersonal_inven_N29] default (0) for [N29],
	
	constraint [DF_tbl_aminepersonal_K30] default (0xFFFFFFFF) for [K30], constraint [DF_tbl_aminepersonal_inven_N30] default (0) for [N30],
	constraint [DF_tbl_aminepersonal_K31] default (0xFFFFFFFF) for [K31], constraint [DF_tbl_aminepersonal_inven_N31] default (0) for [N31],
	constraint [DF_tbl_aminepersonal_K32] default (0xFFFFFFFF) for [K32], constraint [DF_tbl_aminepersonal_inven_N32] default (0) for [N32],
	constraint [DF_tbl_aminepersonal_K33] default (0xFFFFFFFF) for [K33], constraint [DF_tbl_aminepersonal_inven_N33] default (0) for [N33],
	constraint [DF_tbl_aminepersonal_K34] default (0xFFFFFFFF) for [K34], constraint [DF_tbl_aminepersonal_inven_N34] default (0) for [N34],
	constraint [DF_tbl_aminepersonal_K35] default (0xFFFFFFFF) for [K35], constraint [DF_tbl_aminepersonal_inven_N35] default (0) for [N35],
	constraint [DF_tbl_aminepersonal_K36] default (0xFFFFFFFF) for [K36], constraint [DF_tbl_aminepersonal_inven_N36] default (0) for [N36],
	constraint [DF_tbl_aminepersonal_K37] default (0xFFFFFFFF) for [K37], constraint [DF_tbl_aminepersonal_inven_N37] default (0) for [N37],
	constraint [DF_tbl_aminepersonal_K38] default (0xFFFFFFFF) for [K38], constraint [DF_tbl_aminepersonal_inven_N38] default (0) for [N38],
	constraint [DF_tbl_aminepersonal_K39] default (0xFFFFFFFF) for [K39], constraint [DF_tbl_aminepersonal_inven_N39] default (0) for [N39]







GO
/****** Object:  StoredProcedure [dbo].[pcreate_automine]    Script Date: 12/01/2026 16:57:20 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO






create procedure [dbo].[pcreate_automine] 
as
create table [dbo].[tbl_automine_inven] (
	[serial] int identity(1, 1) NOT NULL,
	[dck] [bit] NOT NULL,
	[Race] [tinyint] NOT NULL ,
	[CollisionType] [tinyint] NOT NULL,
	[GuildSerial] [int] NOT NULL ,
	[bWorking] [bit] NOT NULL ,
	[SelectOre] [tinyint] NOT NULL ,
	[Battery] [int] NOT NULL,
	[tmReg] [datetime] NOT NULL,
	[tmLastUpdate] [datetime] NOT NULL,
-- 0 ~ 9
	[K_0] [int] NOT NULL, [O_0] [tinyint] NOT NULL, [K_1] [int] NOT NULL, [O_1] [tinyint] NOT NULL,
	[K_2] [int] NOT NULL, [O_2] [tinyint] NOT NULL, [K_3] [int] NOT NULL, [O_3] [tinyint] NOT NULL,
	[K_4] [int] NOT NULL, [O_4] [tinyint] NOT NULL, [K_5] [int] NOT NULL, [O_5] [tinyint] NOT NULL,
	[K_6] [int] NOT NULL, [O_6] [tinyint] NOT NULL, [K_7] [int] NOT NULL, [O_7] [tinyint] NOT NULL,
	[K_8] [int] NOT NULL, [O_8] [tinyint] NOT NULL, [K_9] [int] NOT NULL, [O_9] [tinyint] NOT NULL,
-- 10 ~ 19
	[K_10] [int] NOT NULL, [O_10] [tinyint] NOT NULL, [K_11] [int] NOT NULL, [O_11] [tinyint] NOT NULL,
	[K_12] [int] NOT NULL, [O_12] [tinyint] NOT NULL, [K_13] [int] NOT NULL, [O_13] [tinyint] NOT NULL,
	[K_14] [int] NOT NULL, [O_14] [tinyint] NOT NULL, [K_15] [int] NOT NULL, [O_15] [tinyint] NOT NULL,
	[K_16] [int] NOT NULL, [O_16] [tinyint] NOT NULL, [K_17] [int] NOT NULL, [O_17] [tinyint] NOT NULL,
	[K_18] [int] NOT NULL, [O_18] [tinyint] NOT NULL, [K_19] [int] NOT NULL, [O_19] [tinyint] NOT NULL,
-- 20 ~ 29
	[K_20] [int] NOT NULL, [O_20] [tinyint] NOT NULL, [K_21] [int] NOT NULL, [O_21] [tinyint] NOT NULL,
	[K_22] [int] NOT NULL, [O_22] [tinyint] NOT NULL, [K_23] [int] NOT NULL, [O_23] [tinyint] NOT NULL,
	[K_24] [int] NOT NULL, [O_24] [tinyint] NOT NULL, [K_25] [int] NOT NULL, [O_25] [tinyint] NOT NULL,
	[K_26] [int] NOT NULL, [O_26] [tinyint] NOT NULL, [K_27] [int] NOT NULL, [O_27] [tinyint] NOT NULL,
	[K_28] [int] NOT NULL, [O_28] [tinyint] NOT NULL, [K_29] [int] NOT NULL, [O_29] [tinyint] NOT NULL,
-- 30 ~ 39
	[K_30] [int] NOT NULL, [O_30] [tinyint] NOT NULL, [K_31] [int] NOT NULL, [O_31] [tinyint] NOT NULL,
	[K_32] [int] NOT NULL, [O_32] [tinyint] NOT NULL, [K_33] [int] NOT NULL, [O_33] [tinyint] NOT NULL,
	[K_34] [int] NOT NULL, [O_34] [tinyint] NOT NULL, [K_35] [int] NOT NULL, [O_35] [tinyint] NOT NULL,
	[K_36] [int] NOT NULL, [O_36] [tinyint] NOT NULL, [K_37] [int] NOT NULL, [O_37] [tinyint] NOT NULL,
	[K_38] [int] NOT NULL, [O_38] [tinyint] NOT NULL, [K_39] [int] NOT NULL, [O_39] [tinyint] NOT NULL,
-- 40 ~ 49
	[K_40] [int] NOT NULL, [O_40] [tinyint] NOT NULL, [K_41] [int] NOT NULL, [O_41] [tinyint] NOT NULL,
	[K_42] [int] NOT NULL, [O_42] [tinyint] NOT NULL, [K_43] [int] NOT NULL, [O_43] [tinyint] NOT NULL,
	[K_44] [int] NOT NULL, [O_44] [tinyint] NOT NULL, [K_45] [int] NOT NULL, [O_45] [tinyint] NOT NULL,
	[K_46] [int] NOT NULL, [O_46] [tinyint] NOT NULL, [K_47] [int] NOT NULL, [O_47] [tinyint] NOT NULL,
	[K_48] [int] NOT NULL, [O_48] [tinyint] NOT NULL, [K_49] [int] NOT NULL, [O_49] [tinyint] NOT NULL,
-- 50 ~ 59
	[K_50] [int] NOT NULL, [O_50] [tinyint] NOT NULL, [K_51] [int] NOT NULL, [O_51] [tinyint] NOT NULL,
	[K_52] [int] NOT NULL, [O_52] [tinyint] NOT NULL, [K_53] [int] NOT NULL, [O_53] [tinyint] NOT NULL,
	[K_54] [int] NOT NULL, [O_54] [tinyint] NOT NULL, [K_55] [int] NOT NULL, [O_55] [tinyint] NOT NULL,
	[K_56] [int] NOT NULL, [O_56] [tinyint] NOT NULL, [K_57] [int] NOT NULL, [O_57] [tinyint] NOT NULL,
	[K_58] [int] NOT NULL, [O_58] [tinyint] NOT NULL, [K_59] [int] NOT NULL, [O_59] [tinyint] NOT NULL,
-- 60 ~ 69
	[K_60] [int] NOT NULL, [O_60] [tinyint] NOT NULL, [K_61] [int] NOT NULL, [O_61] [tinyint] NOT NULL,
	[K_62] [int] NOT NULL, [O_62] [tinyint] NOT NULL, [K_63] [int] NOT NULL, [O_63] [tinyint] NOT NULL,
	[K_64] [int] NOT NULL, [O_64] [tinyint] NOT NULL, [K_65] [int] NOT NULL, [O_65] [tinyint] NOT NULL,
	[K_66] [int] NOT NULL, [O_66] [tinyint] NOT NULL, [K_67] [int] NOT NULL, [O_67] [tinyint] NOT NULL,
	[K_68] [int] NOT NULL, [O_68] [tinyint] NOT NULL, [K_69] [int] NOT NULL, [O_69] [tinyint] NOT NULL,
-- 70 ~ 79
	[K_70] [int] NOT NULL, [O_70] [tinyint] NOT NULL, [K_71] [int] NOT NULL, [O_71] [tinyint] NOT NULL,
	[K_72] [int] NOT NULL, [O_72] [tinyint] NOT NULL, [K_73] [int] NOT NULL, [O_73] [tinyint] NOT NULL,
	[K_74] [int] NOT NULL, [O_74] [tinyint] NOT NULL, [K_75] [int] NOT NULL, [O_75] [tinyint] NOT NULL,
	[K_76] [int] NOT NULL, [O_76] [tinyint] NOT NULL, [K_77] [int] NOT NULL, [O_77] [tinyint] NOT NULL,
	[K_78] [int] NOT NULL, [O_78] [tinyint] NOT NULL, [K_79] [int] NOT NULL, [O_79] [tinyint] NOT NULL
) ON [PRIMARY]

alter table [dbo].[tbl_automine_inven] with nocheck add
	CONSTRAINT [PK_tbl_automine_inven] PRIMARY KEY  CLUSTERED 
	(
		[serial],
		[dck],
		[Race],
		[CollisionType],
		[GuildSerial]
	)  ON [PRIMARY] 

ALTER TABLE [dbo].[tbl_automine_inven] WITH NOCHECK ADD 
	CONSTRAINT [DF_tbl_automine_inven_dck] DEFAULT (0) FOR [dck],
	CONSTRAINT [DF_tbl_automine_inven_Race] DEFAULT (0xFF) FOR [Race],
	CONSTRAINT [DF_tbl_automine_inven_CollisionType] DEFAULT (0xFF) FOR [CollisionType],
	CONSTRAINT [DF_tbl_automine_inven_GuildSerial] DEFAULT (0xFFFFFFFF) FOR [GuildSerial],
	CONSTRAINT [DF_tbl_automine_inven_bWorking] DEFAULT (0) FOR [bWorking],
	CONSTRAINT [DF_tbl_automine_inven_SelectOre] DEFAULT (0) FOR [SelectOre],
	CONSTRAINT [DF_tbl_automine_inven_Battery] DEFAULT (0) FOR [Battery],
	CONSTRAINT [DF_tbl_automine_inven_LastUpdate] DEFAULT (getdate()) FOR [tmLastUpdate],
	CONSTRAINT [DF_tbl_automine_inven_Reg] DEFAULT (getdate()) FOR [tmReg],

	CONSTRAINT [DF__tbl_automin__K_0] DEFAULT (0xFFFFFFFF) FOR [K_0], CONSTRAINT [DF_tbl_automine_inven_O_0] DEFAULT (0) FOR [O_0],
	CONSTRAINT [DF__tbl_automin__K_1] DEFAULT (0xFFFFFFFF) FOR [K_1], CONSTRAINT [DF_tbl_automine_inven_O_1] DEFAULT (0) FOR [O_1],
	CONSTRAINT [DF__tbl_automin__K_2] DEFAULT (0xFFFFFFFF) FOR [K_2], CONSTRAINT [DF_tbl_automine_inven_O_2] DEFAULT (0) FOR [O_2],
	CONSTRAINT [DF__tbl_automin__K_3] DEFAULT (0xFFFFFFFF) FOR [K_3], CONSTRAINT [DF_tbl_automine_inven_O_3] DEFAULT (0) FOR [O_3],
	CONSTRAINT [DF__tbl_automin__K_4] DEFAULT (0xFFFFFFFF) FOR [K_4], CONSTRAINT [DF_tbl_automine_inven_O_4] DEFAULT (0) FOR [O_4],
	CONSTRAINT [DF__tbl_automin__K_5] DEFAULT (0xFFFFFFFF) FOR [K_5], CONSTRAINT [DF_tbl_automine_inven_O_5] DEFAULT (0) FOR [O_5],
	CONSTRAINT [DF__tbl_automin__K_6] DEFAULT (0xFFFFFFFF) FOR [K_6], CONSTRAINT [DF_tbl_automine_inven_O_6] DEFAULT (0) FOR [O_6],
	CONSTRAINT [DF__tbl_automin__K_7] DEFAULT (0xFFFFFFFF) FOR [K_7], CONSTRAINT [DF_tbl_automine_inven_O_7] DEFAULT (0) FOR [O_7],
	CONSTRAINT [DF__tbl_automin__K_8] DEFAULT (0xFFFFFFFF) FOR [K_8], CONSTRAINT [DF_tbl_automine_inven_O_8] DEFAULT (0) FOR [O_8],
	CONSTRAINT [DF__tbl_automin__K_9] DEFAULT (0xFFFFFFFF) FOR [K_9], CONSTRAINT [DF_tbl_automine_inven_O_9] DEFAULT (0) FOR [O_9],
	
	CONSTRAINT [DF__tbl_automin__K_10] DEFAULT (0xFFFFFFFF) FOR [K_10], CONSTRAINT [DF_tbl_automine_inven_O_10] DEFAULT (0) FOR [O_10],
	CONSTRAINT [DF__tbl_automin__K_11] DEFAULT (0xFFFFFFFF) FOR [K_11], CONSTRAINT [DF_tbl_automine_inven_O_11] DEFAULT (0) FOR [O_11],
	CONSTRAINT [DF__tbl_automin__K_12] DEFAULT (0xFFFFFFFF) FOR [K_12], CONSTRAINT [DF_tbl_automine_inven_O_12] DEFAULT (0) FOR [O_12],
	CONSTRAINT [DF__tbl_automin__K_13] DEFAULT (0xFFFFFFFF) FOR [K_13], CONSTRAINT [DF_tbl_automine_inven_O_13] DEFAULT (0) FOR [O_13],
	CONSTRAINT [DF__tbl_automin__K_14] DEFAULT (0xFFFFFFFF) FOR [K_14], CONSTRAINT [DF_tbl_automine_inven_O_14] DEFAULT (0) FOR [O_14],
	CONSTRAINT [DF__tbl_automin__K_15] DEFAULT (0xFFFFFFFF) FOR [K_15], CONSTRAINT [DF_tbl_automine_inven_O_15] DEFAULT (0) FOR [O_15],
	CONSTRAINT [DF__tbl_automin__K_16] DEFAULT (0xFFFFFFFF) FOR [K_16], CONSTRAINT [DF_tbl_automine_inven_O_16] DEFAULT (0) FOR [O_16],
	CONSTRAINT [DF__tbl_automin__K_17] DEFAULT (0xFFFFFFFF) FOR [K_17], CONSTRAINT [DF_tbl_automine_inven_O_17] DEFAULT (0) FOR [O_17],
	CONSTRAINT [DF__tbl_automin__K_18] DEFAULT (0xFFFFFFFF) FOR [K_18], CONSTRAINT [DF_tbl_automine_inven_O_18] DEFAULT (0) FOR [O_18],
	CONSTRAINT [DF__tbl_automin__K_19] DEFAULT (0xFFFFFFFF) FOR [K_19], CONSTRAINT [DF_tbl_automine_inven_O_19] DEFAULT (0) FOR [O_19],
	
	CONSTRAINT [DF__tbl_automin__K_20] DEFAULT (0xFFFFFFFF) FOR [K_20], CONSTRAINT [DF_tbl_automine_inven_O_20] DEFAULT (0) FOR [O_20],
	CONSTRAINT [DF__tbl_automin__K_21] DEFAULT (0xFFFFFFFF) FOR [K_21], CONSTRAINT [DF_tbl_automine_inven_O_21] DEFAULT (0) FOR [O_21],
	CONSTRAINT [DF__tbl_automin__K_22] DEFAULT (0xFFFFFFFF) FOR [K_22], CONSTRAINT [DF_tbl_automine_inven_O_22] DEFAULT (0) FOR [O_22],
	CONSTRAINT [DF__tbl_automin__K_23] DEFAULT (0xFFFFFFFF) FOR [K_23], CONSTRAINT [DF_tbl_automine_inven_O_23] DEFAULT (0) FOR [O_23],
	CONSTRAINT [DF__tbl_automin__K_24] DEFAULT (0xFFFFFFFF) FOR [K_24], CONSTRAINT [DF_tbl_automine_inven_O_24] DEFAULT (0) FOR [O_24],
	CONSTRAINT [DF__tbl_automin__K_25] DEFAULT (0xFFFFFFFF) FOR [K_25], CONSTRAINT [DF_tbl_automine_inven_O_25] DEFAULT (0) FOR [O_25],
	CONSTRAINT [DF__tbl_automin__K_26] DEFAULT (0xFFFFFFFF) FOR [K_26], CONSTRAINT [DF_tbl_automine_inven_O_26] DEFAULT (0) FOR [O_26],
	CONSTRAINT [DF__tbl_automin__K_27] DEFAULT (0xFFFFFFFF) FOR [K_27], CONSTRAINT [DF_tbl_automine_inven_O_27] DEFAULT (0) FOR [O_27],
	CONSTRAINT [DF__tbl_automin__K_28] DEFAULT (0xFFFFFFFF) FOR [K_28], CONSTRAINT [DF_tbl_automine_inven_O_28] DEFAULT (0) FOR [O_28],
	CONSTRAINT [DF__tbl_automin__K_29] DEFAULT (0xFFFFFFFF) FOR [K_29], CONSTRAINT [DF_tbl_automine_inven_O_29] DEFAULT (0) FOR [O_29],
	
	CONSTRAINT [DF__tbl_automin__K_30] DEFAULT (0xFFFFFFFF) FOR [K_30], CONSTRAINT [DF_tbl_automine_inven_O_30] DEFAULT (0) FOR [O_30],
	CONSTRAINT [DF__tbl_automin__K_31] DEFAULT (0xFFFFFFFF) FOR [K_31], CONSTRAINT [DF_tbl_automine_inven_O_31] DEFAULT (0) FOR [O_31],
	CONSTRAINT [DF__tbl_automin__K_32] DEFAULT (0xFFFFFFFF) FOR [K_32], CONSTRAINT [DF_tbl_automine_inven_O_32] DEFAULT (0) FOR [O_32],
	CONSTRAINT [DF__tbl_automin__K_33] DEFAULT (0xFFFFFFFF) FOR [K_33], CONSTRAINT [DF_tbl_automine_inven_O_33] DEFAULT (0) FOR [O_33],
	CONSTRAINT [DF__tbl_automin__K_34] DEFAULT (0xFFFFFFFF) FOR [K_34], CONSTRAINT [DF_tbl_automine_inven_O_34] DEFAULT (0) FOR [O_34],
	CONSTRAINT [DF__tbl_automin__K_35] DEFAULT (0xFFFFFFFF) FOR [K_35], CONSTRAINT [DF_tbl_automine_inven_O_35] DEFAULT (0) FOR [O_35],
	CONSTRAINT [DF__tbl_automin__K_36] DEFAULT (0xFFFFFFFF) FOR [K_36], CONSTRAINT [DF_tbl_automine_inven_O_36] DEFAULT (0) FOR [O_36],
	CONSTRAINT [DF__tbl_automin__K_37] DEFAULT (0xFFFFFFFF) FOR [K_37], CONSTRAINT [DF_tbl_automine_inven_O_37] DEFAULT (0) FOR [O_37],
	CONSTRAINT [DF__tbl_automin__K_38] DEFAULT (0xFFFFFFFF) FOR [K_38], CONSTRAINT [DF_tbl_automine_inven_O_38] DEFAULT (0) FOR [O_38],
	CONSTRAINT [DF__tbl_automin__K_39] DEFAULT (0xFFFFFFFF) FOR [K_39], CONSTRAINT [DF_tbl_automine_inven_O_39] DEFAULT (0) FOR [O_39],
	
	CONSTRAINT [DF__tbl_automin__K_40] DEFAULT (0xFFFFFFFF) FOR [K_40], CONSTRAINT [DF_tbl_automine_inven_O_40] DEFAULT (0) FOR [O_40],
	CONSTRAINT [DF__tbl_automin__K_41] DEFAULT (0xFFFFFFFF) FOR [K_41], CONSTRAINT [DF_tbl_automine_inven_O_41] DEFAULT (0) FOR [O_41],
	CONSTRAINT [DF__tbl_automin__K_42] DEFAULT (0xFFFFFFFF) FOR [K_42], CONSTRAINT [DF_tbl_automine_inven_O_42] DEFAULT (0) FOR [O_42],
	CONSTRAINT [DF__tbl_automin__K_43] DEFAULT (0xFFFFFFFF) FOR [K_43], CONSTRAINT [DF_tbl_automine_inven_O_43] DEFAULT (0) FOR [O_43],
	CONSTRAINT [DF__tbl_automin__K_44] DEFAULT (0xFFFFFFFF) FOR [K_44], CONSTRAINT [DF_tbl_automine_inven_O_44] DEFAULT (0) FOR [O_44],
	CONSTRAINT [DF__tbl_automin__K_45] DEFAULT (0xFFFFFFFF) FOR [K_45], CONSTRAINT [DF_tbl_automine_inven_O_45] DEFAULT (0) FOR [O_45],
	CONSTRAINT [DF__tbl_automin__K_46] DEFAULT (0xFFFFFFFF) FOR [K_46], CONSTRAINT [DF_tbl_automine_inven_O_46] DEFAULT (0) FOR [O_46],
	CONSTRAINT [DF__tbl_automin__K_47] DEFAULT (0xFFFFFFFF) FOR [K_47], CONSTRAINT [DF_tbl_automine_inven_O_47] DEFAULT (0) FOR [O_47],
	CONSTRAINT [DF__tbl_automin__K_48] DEFAULT (0xFFFFFFFF) FOR [K_48], CONSTRAINT [DF_tbl_automine_inven_O_48] DEFAULT (0) FOR [O_48],
	CONSTRAINT [DF__tbl_automin__K_49] DEFAULT (0xFFFFFFFF) FOR [K_49], CONSTRAINT [DF_tbl_automine_inven_O_49] DEFAULT (0) FOR [O_49],
	
	CONSTRAINT [DF__tbl_automin__K_50] DEFAULT (0xFFFFFFFF) FOR [K_50], CONSTRAINT [DF_tbl_automine_inven_O_50] DEFAULT (0) FOR [O_50],
	CONSTRAINT [DF__tbl_automin__K_51] DEFAULT (0xFFFFFFFF) FOR [K_51], CONSTRAINT [DF_tbl_automine_inven_O_51] DEFAULT (0) FOR [O_51],
	CONSTRAINT [DF__tbl_automin__K_52] DEFAULT (0xFFFFFFFF) FOR [K_52], CONSTRAINT [DF_tbl_automine_inven_O_52] DEFAULT (0) FOR [O_52],
	CONSTRAINT [DF__tbl_automin__K_53] DEFAULT (0xFFFFFFFF) FOR [K_53], CONSTRAINT [DF_tbl_automine_inven_O_53] DEFAULT (0) FOR [O_53],
	CONSTRAINT [DF__tbl_automin__K_54] DEFAULT (0xFFFFFFFF) FOR [K_54], CONSTRAINT [DF_tbl_automine_inven_O_54] DEFAULT (0) FOR [O_54],
	CONSTRAINT [DF__tbl_automin__K_55] DEFAULT (0xFFFFFFFF) FOR [K_55], CONSTRAINT [DF_tbl_automine_inven_O_55] DEFAULT (0) FOR [O_55],
	CONSTRAINT [DF__tbl_automin__K_56] DEFAULT (0xFFFFFFFF) FOR [K_56], CONSTRAINT [DF_tbl_automine_inven_O_56] DEFAULT (0) FOR [O_56],
	CONSTRAINT [DF__tbl_automin__K_57] DEFAULT (0xFFFFFFFF) FOR [K_57], CONSTRAINT [DF_tbl_automine_inven_O_57] DEFAULT (0) FOR [O_57],
	CONSTRAINT [DF__tbl_automin__K_58] DEFAULT (0xFFFFFFFF) FOR [K_58], CONSTRAINT [DF_tbl_automine_inven_O_58] DEFAULT (0) FOR [O_58],
	CONSTRAINT [DF__tbl_automin__K_59] DEFAULT (0xFFFFFFFF) FOR [K_59], CONSTRAINT [DF_tbl_automine_inven_O_59] DEFAULT (0) FOR [O_59],
	
	CONSTRAINT [DF__tbl_automin__K_60] DEFAULT (0xFFFFFFFF) FOR [K_60], CONSTRAINT [DF_tbl_automine_inven_O_60] DEFAULT (0) FOR [O_60],
	CONSTRAINT [DF__tbl_automin__K_61] DEFAULT (0xFFFFFFFF) FOR [K_61], CONSTRAINT [DF_tbl_automine_inven_O_61] DEFAULT (0) FOR [O_61],
	CONSTRAINT [DF__tbl_automin__K_62] DEFAULT (0xFFFFFFFF) FOR [K_62], CONSTRAINT [DF_tbl_automine_inven_O_62] DEFAULT (0) FOR [O_62],
	CONSTRAINT [DF__tbl_automin__K_63] DEFAULT (0xFFFFFFFF) FOR [K_63], CONSTRAINT [DF_tbl_automine_inven_O_63] DEFAULT (0) FOR [O_63],
	CONSTRAINT [DF__tbl_automin__K_64] DEFAULT (0xFFFFFFFF) FOR [K_64], CONSTRAINT [DF_tbl_automine_inven_O_64] DEFAULT (0) FOR [O_64],
	CONSTRAINT [DF__tbl_automin__K_65] DEFAULT (0xFFFFFFFF) FOR [K_65], CONSTRAINT [DF_tbl_automine_inven_O_65] DEFAULT (0) FOR [O_65],
	CONSTRAINT [DF__tbl_automin__K_66] DEFAULT (0xFFFFFFFF) FOR [K_66], CONSTRAINT [DF_tbl_automine_inven_O_66] DEFAULT (0) FOR [O_66],
	CONSTRAINT [DF__tbl_automin__K_67] DEFAULT (0xFFFFFFFF) FOR [K_67], CONSTRAINT [DF_tbl_automine_inven_O_67] DEFAULT (0) FOR [O_67],
	CONSTRAINT [DF__tbl_automin__K_68] DEFAULT (0xFFFFFFFF) FOR [K_68], CONSTRAINT [DF_tbl_automine_inven_O_68] DEFAULT (0) FOR [O_68],
	CONSTRAINT [DF__tbl_automin__K_69] DEFAULT (0xFFFFFFFF) FOR [K_69], CONSTRAINT [DF_tbl_automine_inven_O_69] DEFAULT (0) FOR [O_69],

	CONSTRAINT [DF__tbl_automin__K_70] DEFAULT (0xFFFFFFFF) FOR [K_70], CONSTRAINT [DF_tbl_automine_inven_O_70] DEFAULT (0) FOR [O_70],
	CONSTRAINT [DF__tbl_automin__K_71] DEFAULT (0xFFFFFFFF) FOR [K_71], CONSTRAINT [DF_tbl_automine_inven_O_71] DEFAULT (0) FOR [O_71],
	CONSTRAINT [DF__tbl_automin__K_72] DEFAULT (0xFFFFFFFF) FOR [K_72], CONSTRAINT [DF_tbl_automine_inven_O_72] DEFAULT (0) FOR [O_72],
	CONSTRAINT [DF__tbl_automin__K_73] DEFAULT (0xFFFFFFFF) FOR [K_73], CONSTRAINT [DF_tbl_automine_inven_O_73] DEFAULT (0) FOR [O_73],
	CONSTRAINT [DF__tbl_automin__K_74] DEFAULT (0xFFFFFFFF) FOR [K_74], CONSTRAINT [DF_tbl_automine_inven_O_74] DEFAULT (0) FOR [O_74],
	CONSTRAINT [DF__tbl_automin__K_75] DEFAULT (0xFFFFFFFF) FOR [K_75], CONSTRAINT [DF_tbl_automine_inven_O_75] DEFAULT (0) FOR [O_75],
	CONSTRAINT [DF__tbl_automin__K_76] DEFAULT (0xFFFFFFFF) FOR [K_76], CONSTRAINT [DF_tbl_automine_inven_O_76] DEFAULT (0) FOR [O_76],
	CONSTRAINT [DF__tbl_automin__K_77] DEFAULT (0xFFFFFFFF) FOR [K_77], CONSTRAINT [DF_tbl_automine_inven_O_77] DEFAULT (0) FOR [O_77],
	CONSTRAINT [DF__tbl_automin__K_78] DEFAULT (0xFFFFFFFF) FOR [K_78], CONSTRAINT [DF_tbl_automine_inven_O_78] DEFAULT (0) FOR [O_78],
	CONSTRAINT [DF__tbl_automin__K_79] DEFAULT (0xFFFFFFFF) FOR [K_79], CONSTRAINT [DF_tbl_automine_inven_O_79] DEFAULT (0) FOR [O_79]







GO
/****** Object:  StoredProcedure [dbo].[pDelete_AvatorData]    Script Date: 12/01/2026 16:57:20 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO





/*
캐릭터데이터를 실제로 삭제한다
*/
CREATE PROCEDURE [dbo].[pDelete_AvatorData]
@serial int
AS
begin tran
delete tbl_base where serial=@serial
IF @@rowcount <> 1
begin
    rollback tran
    return
end

select 'Delete Base Data'

delete [dbo].[tbl_general] where serial=@serial
IF @@rowcount <> 1
begin
    rollback tran
    return
end

select 'Delete General Data'

delete [dbo].[tbl_inven] where serial=@serial
IF @@rowcount <> 1
begin
    rollback tran
    return
end

select 'Delete Inven Data'

delete [dbo].[tbl_quest] where serial=@serial
IF @@rowcount <> 1
begin
    rollback tran
    return
end

select 'Delete Quest Data'

delete [dbo].[tbl_userinterface] where serial=@serial
IF @@rowcount <> 1
begin
    rollback tran
    return
end

select 'Delete UI Data'

if exists(select * from [dbo].[tbl_unit] where serial=@serial) 
begin
    delete [dbo].[tbl_unit] where serial=@serial
    select 'Delete Unit Data'
end

commit tran

select 'Delete Avator Data Successfull'






GO
/****** Object:  StoredProcedure [dbo].[pDelete_AvatorData_NoSelect]    Script Date: 12/01/2026 16:57:20 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO





/*
캐릭터데이터를 실제로 삭제한다
*/
CREATE PROCEDURE [dbo].[pDelete_AvatorData_NoSelect]
@serial int
AS
begin tran
delete [dbo].[tbl_base] where serial=@serial
IF @@rowcount <> 1
begin
    rollback tran
    return
end

delete [dbo].[tbl_general] where serial=@serial
IF @@rowcount <> 1
begin
    rollback tran
    return
end

delete [dbo].[tbl_inven] where serial=@serial
IF @@rowcount <> 1
begin
    rollback tran
    return
end

delete [dbo].[tbl_quest] where serial=@serial

delete [dbo].[tbl_userinterface] where serial=@serial

if exists(select * from [dbo].[tbl_unit] where serial=@serial) 
begin
    delete [dbo].[tbl_unit] where serial=@serial
end

commit tran






GO
/****** Object:  StoredProcedure [dbo].[pDelete_Base]    Script Date: 12/01/2026 16:57:20 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO






/*
캐릭터를 지운다(DCK를 1로만...)
*/
CREATE PROCEDURE [dbo].[pDelete_Base]
@serial int
AS
UPDATE [dbo].[tbl_base] SET DCK = 1,  Arrange = 1, DeleteTime = getdate()  WHERE Serial = @serial






GO
/****** Object:  StoredProcedure [dbo].[pDelete_CombineEx_Result]    Script Date: 12/01/2026 16:57:20 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO





--
CREATE PROCEDURE [dbo].[pDelete_CombineEx_Result]
@serial int
AS
UPDATE [dbo].[tbl_itemcombine_ex_result] SET DCK = 1 WHERE Serial = @serial






GO
/****** Object:  StoredProcedure [dbo].[pDelete_General]    Script Date: 12/01/2026 16:57:20 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO






/****** 개체: 저장 프로시저 dbo.pDelete_Avator_Stat    스크립트 날짜: 2003-04-10 오후 9:05:30 ******/
CREATE PROCEDURE [dbo].[pDelete_General]
@serial int
AS
UPDATE [dbo].[tbl_general] SET DCK = 1 WHERE Serial = @serial






GO
/****** Object:  StoredProcedure [dbo].[pDelete_Inven]    Script Date: 12/01/2026 16:57:20 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO





/****** 개체: 저장 프로시저 dbo.pDelete_Avator_Inven    스크립트 날짜: 2003-04-10 오후 9:05:30 ******/
CREATE PROCEDURE [dbo].[pDelete_Inven]
@serial int
AS
UPDATE [dbo].[tbl_inven] SET DCK = 1 WHERE Serial = @serial






GO
/****** Object:  StoredProcedure [dbo].[pDelete_ItemCharge]    Script Date: 12/01/2026 16:57:20 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO





/*
아이템 증여목록에서 하나를 지운다
*/
CREATE PROCEDURE [dbo].[pDelete_ItemCharge]
@serial int
AS
UPDATE [dbo].[tbl_ItemCharge] SET DCK = 1, dtTakeDate=(getDate()) WHERE nSerial = @serial






GO
/****** Object:  StoredProcedure [dbo].[pDelete_PatriarchComm]    Script Date: 12/01/2026 16:57:20 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO





/*
-- 인덱스 추가
-- 작성자 : 이희길
-- 작성일 : 2007-02-27(화)
-- 내용 : 프로시져에서 참고시 가장 많은 필드인 avatorserial, depositdate에 인덱스 추가
CREATE INDEX [IX_tbl_patriarch_comm_avatorserial] on [dbo].[tbl_patriarch_comm](avatorserial)
CREATE INDEX [IX_tbl_patriarch_comm_depositdate] on [dbo].[tbl_patriarch_comm](depositdate)
*/



/*
   작성자 : 김태완
   작성일 : 2007-02-08 (목)
   내용 : 출금이 이루어진 경우 입금 레코드 출금 처리
*/
CREATE PROCEDURE [dbo].[pDelete_PatriarchComm]
@aserial int,
@depdate char(8)
AS

UPDATE	[dbo].[tbl_patriarch_comm] 
SET 	DCK=1, PayDate = getdate()
WHERE	AvatorSerial = @aserial
AND	DepositDate = @depdate
AND	DCK = 0






GO
/****** Object:  StoredProcedure [dbo].[pDelete_TrunkCharge]    Script Date: 12/01/2026 16:57:20 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO





/*
창고 증여목록에서 하나를 지운다
*/
CREATE PROCEDURE [dbo].[pDelete_TrunkCharge]
@id int
as
update [dbo].[tbl_AccountTrunkCharge] set DCK = 1, TakeDate = GETDATE()
from [dbo].[tbl_AccountTrunkCharge] where ID = @id






GO
/****** Object:  StoredProcedure [dbo].[pDelete_Unit]    Script Date: 12/01/2026 16:57:20 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO






CREATE PROCEDURE [dbo].[pDelete_Unit]
@serial int
AS
UPDATE [dbo].[tbl_inven] SET DCK = 1 WHERE Serial = @serial






GO
/****** Object:  StoredProcedure [dbo].[pDeleteDeleteNotUseCharacterInfo]    Script Date: 12/01/2026 16:57:20 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO

/**
 이전 캐릭터 정보 삭제
*/
CREATE procedure [dbo].[pDeleteDeleteNotUseCharacterInfo]
as
-- tbl_base
declare @3monthbeforedate datetime
declare @curdate datetime
set @curdate = convert( datetime, convert( varchar, getdate(), 112 ) )
set @3monthbeforedate = dateadd( m, -3, @curdate )

-- tbl_general
delete [dbo].[tbl_general] from [dbo].[tbl_general] as g join
( select top 10000
[Serial]
from [dbo].[tbl_base]
where [DCK] = 1 and [Lv] > 0 and [AccountSerial] < 2000000000 and [DeleteTime] < @3monthbeforedate and [Arrange] = 1
order by [Serial] ) as b
on g.[Serial] = b.[Serial]

-- tbl_inven
delete [dbo].[tbl_inven] from [dbo].[tbl_inven] as i join
( select top 10000
[Serial]
from [dbo].[tbl_base]
where [DCK] = 1 and [Lv] > 0 and [AccountSerial] < 2000000000 and [DeleteTime] < @3monthbeforedate and [Arrange] = 1
order by [Serial] ) as b
on i.[Serial] = b.[Serial]

-- tbl_unit
delete [dbo].[tbl_unit] from [dbo].[tbl_unit] as u join
( select top 10000
[Serial]
from [dbo].[tbl_base]
where [DCK] = 1 and [Lv] > 0 and [AccountSerial] < 2000000000 and [DeleteTime] < @3monthbeforedate and [Arrange] = 1
order by [Serial] ) as b
on u.[Serial] = b.[Serial]

-- tbl_quest
delete [dbo].[tbl_quest] from [dbo].[tbl_quest] as q join
( select top 10000
[Serial]
from [dbo].[tbl_base]
where [DCK] = 1 and [Lv] > 0 and [AccountSerial] < 2000000000 and [DeleteTime] < @3monthbeforedate and [Arrange] = 1
order by [Serial] ) as b
on q.[Serial] = b.[Serial]

-- tbl_userinterface
delete [dbo].[tbl_userinterface] from [dbo].[tbl_userinterface] as ui join
( select top 10000
[Serial]
from [dbo].[tbl_base]
where [DCK] = 1 and [Lv] > 0 and [AccountSerial] < 2000000000 and [DeleteTime] < @3monthbeforedate and [Arrange] = 1
order by [Serial] ) as b
on ui.[Serial] = b.[Serial]


-- tbl_aminepersonal_inven
delete [dbo].[tbl_aminepersonal_inven] from [dbo].[tbl_aminepersonal_inven] as ampi join
( select top 10000
[Serial]
from [dbo].[tbl_base]
where [DCK] = 1 and [Lv] > 0 and [AccountSerial] < 2000000000 and [DeleteTime] < @3monthbeforedate and [Arrange] = 1
order by [Serial] ) as b
on ampi.[avatorserial] = b.[serial]


-- tbl_itemcharge
delete [dbo].[tbl_itemcharge] from [dbo].[tbl_itemcharge] as ic join
( select top 10000
[Serial]
from [dbo].[tbl_base]
where [DCK] = 1 and [Lv] > 0 and [AccountSerial] < 2000000000 and [DeleteTime] < @3monthbeforedate and [Arrange] = 1
order by [Serial] ) as b
on ic.[nAvatorSerial] = b.[Serial]
where ic.[DCK] = 0


-- tbl_pvporderview
delete [dbo].[tbl_pvporderview] from [dbo].[tbl_pvporderview] as pov join
( select top 10000
[Serial]
from [dbo].[tbl_base]
where [DCK] = 1 and [Lv] > 0 and [AccountSerial] < 2000000000 and [DeleteTime] < @3monthbeforedate and [Arrange] = 1
order by [Serial] ) as b
on pov.[serial] = b.[Serial]


-- tbl_npc_quest_history
delete [dbo].[tbl_npc_quest_history] from [dbo].[tbl_npc_quest_history] as nqh join
( select top 10000
[Serial]
from [dbo].[tbl_base]
where [DCK] = 1 and [Lv] > 0 and [AccountSerial] < 2000000000 and [DeleteTime] < @3monthbeforedate and [Arrange] = 1
order by [Serial] ) as b
on nqh.[serial] = b.[Serial]


-- tbl_potion_delay
delete [dbo].[tbl_potion_delay] from [dbo].[tbl_potion_delay] as pd join
( select top 10000
[Serial]
from [dbo].[tbl_base]
where [DCK] = 1 and [Lv] > 0 and [AccountSerial] < 2000000000 and [DeleteTime] < @3monthbeforedate and [Arrange] = 1
order by [Serial] ) as b
on pd.[Serial] = b.[Serial]


-- tbl_supplement
delete [dbo].[tbl_supplement] from [dbo].[tbl_supplement] as sm join
( select top 10000
[Serial]
from [dbo].[tbl_base]
where [DCK] = 1 and [Lv] > 0 and [AccountSerial] < 2000000000 and [DeleteTime] < @3monthbeforedate and [Arrange] = 1
order by [Serial] ) as b
on sm.[serial] = b.[Serial]

-- tbl_base
delete [dbo].[tbl_base] from [dbo].[tbl_base] as bd join
( select top 10000
[Serial]
from [dbo].[tbl_base]
where [DCK] = 1 and [Lv] > 0 and [AccountSerial] < 2000000000 and [DeleteTime] < @3monthbeforedate and [Arrange] = 1
order by [Serial] ) as bs
on bd.[Serial] = bs.[Serial]




GO
/****** Object:  StoredProcedure [dbo].[pDeleteIntegrateNotUseCharacterInfo]    Script Date: 12/01/2026 16:57:20 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO


/**
 이전 캐릭터 정보 삭제
*/
CREATE procedure [dbo].[pDeleteIntegrateNotUseCharacterInfo]
as
-- tbl_general
delete [dbo].[tbl_general] from [dbo].[tbl_general] as g join
( select top 10000
[Serial]
from [dbo].[tbl_base]
where [DCK] = 1 and [Lv] > 0 and [AccountSerial] < 2000000000  and [Arrange] = 0
order by [Serial] ) as b
on g.[Serial] = b.[Serial]

-- tbl_inven
delete [dbo].[tbl_inven] from [dbo].[tbl_inven] as i join
( select top 10000
[Serial]
from [dbo].[tbl_base]
where [DCK] = 1 and [Lv] > 0 and [AccountSerial] < 2000000000  and [Arrange] = 0
order by [Serial] ) as b
on i.[Serial] = b.[Serial]

-- tbl_unit
delete [dbo].[tbl_unit] from [dbo].[tbl_unit] as u join
( select top 10000
[Serial]
from [dbo].[tbl_base]
where [DCK] = 1 and [Lv] > 0 and [AccountSerial] < 2000000000  and [Arrange] = 0
order by [Serial] ) as b
on u.[Serial] = b.[Serial]

-- tbl_quest
delete [dbo].[tbl_quest] from [dbo].[tbl_quest] as q join
( select top 10000
[Serial]
from [dbo].[tbl_base]
where [DCK] = 1 and [Lv] > 0 and [AccountSerial] < 2000000000  and [Arrange] = 0
order by [Serial] ) as b
on q.[Serial] = b.[Serial]

-- tbl_userinterface
delete [dbo].[tbl_userinterface] from [dbo].[tbl_userinterface] as ui join
( select top 10000
[Serial]
from [dbo].[tbl_base]
where [DCK] = 1 and [Lv] > 0 and [AccountSerial] < 2000000000  and [Arrange] = 0
order by [Serial] ) as b
on ui.[Serial] = b.[Serial]


-- tbl_aminepersonal_inven
delete [dbo].[tbl_aminepersonal_inven] from [dbo].[tbl_aminepersonal_inven] as ampi join
( select top 10000
[Serial]
from [dbo].[tbl_base]
where [DCK] = 1 and [Lv] > 0 and [AccountSerial] < 2000000000  and [Arrange] = 0
order by [Serial] ) as b
on ampi.[avatorserial] = b.[serial]


-- tbl_itemcharge
delete [dbo].[tbl_itemcharge] from [dbo].[tbl_itemcharge] as ic join
( select top 10000
[Serial]
from [dbo].[tbl_base]
where [DCK] = 1 and [Lv] > 0 and [AccountSerial] < 2000000000  and [Arrange] = 0
order by [Serial] ) as b
on ic.[nAvatorSerial] = b.[Serial]
where ic.[DCK] = 0


-- tbl_pvporderview
delete [dbo].[tbl_pvporderview] from [dbo].[tbl_pvporderview] as pov join
( select top 10000
[Serial]
from [dbo].[tbl_base]
where [DCK] = 1 and [Lv] > 0 and [AccountSerial] < 2000000000  and [Arrange] = 0
order by [Serial] ) as b
on pov.[serial] = b.[Serial]


-- tbl_npc_quest_history
delete [dbo].[tbl_npc_quest_history] from [dbo].[tbl_npc_quest_history] as nqh join
( select top 10000
[Serial]
from [dbo].[tbl_base]
where [DCK] = 1 and [Lv] > 0 and [AccountSerial] < 2000000000  and [Arrange] = 0
order by [Serial] ) as b
on nqh.[serial] = b.[Serial]


-- tbl_potion_delay
delete [dbo].[tbl_potion_delay] from [dbo].[tbl_potion_delay] as pd join
( select top 10000
[Serial]
from [dbo].[tbl_base]
where [DCK] = 1 and [Lv] > 0 and [AccountSerial] < 2000000000  and [Arrange] = 0
order by [Serial] ) as b
on pd.[Serial] = b.[Serial]


-- tbl_supplement
delete [dbo].[tbl_supplement] from [dbo].[tbl_supplement] as sm join
( select top 10000
[Serial]
from [dbo].[tbl_base]
where [DCK] = 1 and [Lv] > 0 and [AccountSerial] < 2000000000  and [Arrange] = 0
order by [Serial] ) as b
on sm.[serial] = b.[Serial]

-- tbl_base
delete [dbo].[tbl_base] from [dbo].[tbl_base] as bd join
( select top 10000
[Serial]
from [dbo].[tbl_base]
where [DCK] = 1 and [Lv] > 0 and [AccountSerial] < 2000000000  and [Arrange] = 0
order by [Serial] ) as bs
on bd.[Serial] = bs.[Serial]




GO
/****** Object:  StoredProcedure [dbo].[pDeleteMoveNotUseCharacterInfo]    Script Date: 12/01/2026 16:57:20 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO

/**
 이전된 휴면 캐릭터 정보 삭제
*/
create procedure [dbo].[pDeleteMoveNotUseCharacterInfo]
as  
declare @6monthbefore int  
declare @6monthbeforedate datetime  
declare @curdate datetime  
set @curdate = convert( datetime, convert( varchar, getdate(), 112 ) )  
set @6monthbeforedate = dateadd( m, -6, @curdate )  
set @6MonthBefore = convert( int, substring( replace ( replace( replace( convert( varchar, @6monthbeforedate, 120 ), '-', '' ), ':', ''), ' ', '' ), 4, 9 ) )  
  
-- tbl_general  
delete [dbo].[tbl_general] from [dbo].[tbl_general] as g join  
( select top 10000
[Serial]  
from [dbo].[tbl_base]  
where [DCK] = 0 and [Lv] > 0 and [AccountSerial] < 2000000000 and [LastConnTime] < @6MonthBefore  
order by [Serial] ) as b  
on g.[Serial] = b.[Serial]  
  
-- tbl_inven  
delete [dbo].[tbl_inven] from [dbo].[tbl_inven] as i join  
( select top 10000
[Serial]  
from [dbo].[tbl_base]  
where [DCK] = 0 and [Lv] > 0 and [AccountSerial] < 2000000000 and [LastConnTime] < @6MonthBefore  
order by [Serial] ) as b  
on i.[Serial] = b.[Serial]  
  
-- tbl_unit  
delete [dbo].[tbl_unit] from [dbo].[tbl_unit] as u join  
( select top 10000
[Serial]  
from [dbo].[tbl_base]  
where [DCK] = 0 and [Lv] > 0 and [AccountSerial] < 2000000000 and [LastConnTime] < @6MonthBefore  
order by [Serial] ) as b  
on u.[Serial] = b.[Serial]  
  
-- tbl_quest  
delete [dbo].[tbl_quest] from [dbo].[tbl_quest] as q join  
( select top 10000
[Serial]  
from [dbo].[tbl_base]  
where [DCK] = 0 and [Lv] > 0 and [AccountSerial] < 2000000000 and [LastConnTime] < @6MonthBefore  
order by [Serial] ) as b  
on q.[Serial] = b.[Serial]  
  
-- tbl_userinterface  
delete [dbo].[tbl_userinterface] from [dbo].[tbl_userinterface] as ui join  
( select top 10000
[Serial]  
from [dbo].[tbl_base]  
where [DCK] = 0 and [Lv] > 0 and [AccountSerial] < 2000000000 and [LastConnTime] < @6MonthBefore  
order by [Serial] ) as b  
on ui.[Serial] = b.[Serial]  
  
-- tbl_aminepersonal_inven  
delete [dbo].[tbl_aminepersonal_inven] from [dbo].[tbl_aminepersonal_inven] as ampi join  
( select top 10000
[Serial]  
from [dbo].[tbl_base]  
where [DCK] = 0 and [Lv] > 0 and [AccountSerial] < 2000000000 and [LastConnTime] < @6MonthBefore  
order by [Serial] ) as b  
on ampi.[avatorserial] = b.[serial]  
  
-- tbl_itemcharge  
delete [dbo].[tbl_itemcharge] from [dbo].[tbl_itemcharge] as ic join  
( select top 10000
[Serial]  
from [dbo].[tbl_base]  
where [DCK] = 0 and [Lv] > 0 and [AccountSerial] < 2000000000 and [LastConnTime] < @6MonthBefore  
order by [Serial] ) as b  
on ic.[nAvatorSerial] = b.[Serial]  
where ic.[DCK] = 0  
  
-- tbl_pvporderview  
delete [dbo].[tbl_pvporderview] from [dbo].[tbl_pvporderview] as pov join  
( select top 10000
[Serial]  
from [dbo].[tbl_base]  
where [DCK] = 0 and [Lv] > 0 and [AccountSerial] < 2000000000 and [LastConnTime] < @6MonthBefore  
order by [Serial] ) as b  
on pov.[serial] = b.[Serial]  
  
  
-- tbl_npc_quest_history  
delete [dbo].[tbl_npc_quest_history] from [dbo].[tbl_npc_quest_history] as nqh join  
( select top 10000
[Serial]  
from [dbo].[tbl_base]  
where [DCK] = 0 and [Lv] > 0 and [AccountSerial] < 2000000000 and [LastConnTime] < @6MonthBefore  
order by [Serial] ) as b  
on nqh.[serial] = b.[Serial]  
  
-- tbl_potion_delay  
delete [dbo].[tbl_potion_delay] from [dbo].[tbl_potion_delay] as pd join  
( select top 10000
[Serial]  
from [dbo].[tbl_base]  
where [DCK] = 0 and [Lv] > 0 and [AccountSerial] < 2000000000 and [LastConnTime] < @6MonthBefore  
order by [Serial] ) as b  
on pd.[serial] = b.[Serial]  
  
  
-- tbl_supplement  
delete [dbo].[tbl_supplement] from [dbo].[tbl_supplement] as sm join  
( select top 10000
[Serial]  
from [dbo].[tbl_base]  
where [DCK] = 0 and [Lv] > 0 and [AccountSerial] < 2000000000 and [LastConnTime] < @6MonthBefore  
order by [Serial] ) as b  
on sm.[serial] = b.[Serial]  
  
-- tbl_base  
delete [dbo].[tbl_base] from [dbo].[tbl_base] as bd join  
( select top 10000
[Serial]  
from [dbo].[tbl_base]  
where [DCK] = 0 and [Lv] > 0 and [AccountSerial] < 2000000000 and [LastConnTime] < @6MonthBefore  
order by [Serial] ) as bs  
on bd.[Serial] = bs.[Serial]  




GO
/****** Object:  StoredProcedure [dbo].[pDeleteOutOfAccountCharacter]    Script Date: 12/01/2026 16:57:20 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO


/**
 멤버쉽 탈퇴한 대상 삭제
*/
CREATE procedure [dbo].[pDeleteOutOfAccountCharacter]
as
begin tran

-- tbl_general
delete [dbo].[tbl_general] from [dbo].[tbl_general] as g join
( select top 30 b.[Serial]
from [dbo].[tbl_base] as b
join ( select top 10 [serial] from [dbo].[tbl_leaveaccountlist] ) as lal on b.[AccountSerial] = lal.[serial]
order by b.[Serial] ) as s
on g.[Serial] = s.[Serial]
if( @@rowcount = 0 or @@error <> 0 )
begin
	rollback tran
	return
end

-- tbl_inven
delete [dbo].[tbl_inven] from [dbo].[tbl_inven] as i join
( select top 30 b.[Serial]
from [dbo].[tbl_base] as b
join ( select top 10 [serial] from [dbo].[tbl_leaveaccountlist] ) as lal on b.[AccountSerial] = lal.[serial]
order by b.[Serial] ) as s
on i.[Serial] = s.[Serial]
if( @@rowcount = 0 or @@error <> 0 )
begin
	rollback tran
	return
end

-- tbl_unit
delete [dbo].[tbl_unit] from [dbo].[tbl_unit] as u join
( select top 30 b.[Serial]
from [dbo].[tbl_base] as b
join ( select top 10 [serial] from [dbo].[tbl_leaveaccountlist] ) as lal on b.[AccountSerial] = lal.[serial]
order by b.[Serial] ) as s
on u.[Serial] = s.[Serial]
if( @@error <> 0 )
begin
	rollback tran
	return
end

-- tbl_quest
delete [dbo].[tbl_quest] from [dbo].[tbl_quest] as q join
( select top 30 b.[Serial]
from [dbo].[tbl_base] as b
join ( select top 10 [serial] from [dbo].[tbl_leaveaccountlist] ) as lal on b.[AccountSerial] = lal.[serial]
order by b.[Serial] ) as s
on q.[Serial] = s.[Serial]
if( @@error <> 0 )
begin
	rollback tran
	return
end


-- tbl_userinterface
delete [dbo].[tbl_userinterface] from [dbo].[tbl_userinterface] as ui join
( select top 30 b.[Serial]
from [dbo].[tbl_base] as b
join ( select top 10 [serial] from [dbo].[tbl_leaveaccountlist] ) as lal on b.[AccountSerial] = lal.[serial]
order by b.[Serial] ) as s
on ui.[Serial] = s.[Serial]
if( @@error <> 0 )
begin
	rollback tran
	return
end


-- tbl_aminepersonal_inven
delete [dbo].[tbl_aminepersonal_inven] from [dbo].[tbl_aminepersonal_inven] as ai join
( select top 30 b.[Serial]
from [dbo].[tbl_base] as b
join ( select top 10 [serial] from [dbo].[tbl_leaveaccountlist] ) as lal on b.[AccountSerial] = lal.[serial]
order by b.[Serial] ) as s
on ai.[avatorserial] = s.[Serial]
if( @@error <> 0 )
begin
	rollback tran
	return
end

-- tbl_itemcharge
delete [dbo].[tbl_itemcharge] from [dbo].[tbl_itemcharge] as ic join
( select top 30 b.[Serial]
from [dbo].[tbl_base] as b
join ( select top 10 [serial] from [dbo].[tbl_leaveaccountlist] ) as lal on b.[AccountSerial] = lal.[serial]
order by b.[Serial] ) as s
on ic.[nAvatorSerial] = s.[Serial]
if( @@error <> 0 )
begin
	rollback tran
	return
end

-- tbl_pvporderview
delete [dbo].[tbl_pvporderview] from [dbo].[tbl_pvporderview] as pv join
( select top 30 b.[Serial]
from [dbo].[tbl_base] as b
join ( select top 10 [serial] from [dbo].[tbl_leaveaccountlist] ) as lal on b.[AccountSerial] = lal.[serial]
order by b.[Serial] ) as s
on pv.[Serial] = s.[Serial]
if( @@error <> 0 )
begin
	rollback tran
	return
end

-- tbl_npc_quest_history
delete [dbo].[tbl_npc_quest_history] from [dbo].[tbl_npc_quest_history] as nqh join
( select top 30 b.[Serial]
from [dbo].[tbl_base] as b
join ( select top 10 [serial] from [dbo].[tbl_leaveaccountlist] ) as lal on b.[AccountSerial] = lal.[serial]
order by b.[Serial] ) as s
on nqh.[Serial] = s.[Serial]
if( @@error <> 0 )
begin
	rollback tran
	return
end

-- tbl_potion_delay
delete [dbo].[tbl_potion_delay] from [dbo].[tbl_potion_delay] as pd join
( select top 30 b.[Serial]
from [dbo].[tbl_base] as b
join ( select top 10 [serial] from [dbo].[tbl_leaveaccountlist] ) as lal on b.[AccountSerial] = lal.[serial]
order by b.[Serial] ) as s
on pd.[Serial] = s.[Serial]
if( @@error <> 0 )
begin
	rollback tran
	return
end

-- tbl_supplement
delete [dbo].[tbl_supplement] from [dbo].[tbl_supplement] as sm join
( select top 30 b.[Serial]
from [dbo].[tbl_base] as b
join ( select top 10 [serial] from [dbo].[tbl_leaveaccountlist] ) as lal on b.[AccountSerial] = lal.[serial]
order by b.[Serial] ) as s
on sm.[Serial] = s.[Serial]
if( @@error <> 0 )
begin
	rollback tran
	return
end

commit tran




GO
/****** Object:  StoredProcedure [dbo].[pGet_Serial]    Script Date: 12/01/2026 16:57:20 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO





/****** Object:  Stored Procedure dbo.pGet_Serial    Script Date: 2003-09-27 오후 3:22:15 ******/
CREATE PROCEDURE [dbo].[pGet_Serial]
@name VARCHAR(17)
AS
SELECT Serial, AccountSerial, Account
FROM [dbo].[tbl_base]
WHERE Name = @name AND DCK = 0






GO
/****** Object:  StoredProcedure [dbo].[pInsert_AccountTrunk]    Script Date: 12/01/2026 16:57:20 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO





--
CREATE PROCEDURE [dbo].[pInsert_AccountTrunk]
@serial int
AS
INSERT [dbo].[tbl_AccountTrunk] (AccountSerial )
VALUES (@serial)






GO
/****** Object:  StoredProcedure [dbo].[pInsert_AccountTrunkExtend]    Script Date: 12/01/2026 16:57:20 ******/
SET ANSI_NULLS OFF
GO
SET QUOTED_IDENTIFIER OFF
GO


CREATE PROCEDURE [dbo].[pInsert_AccountTrunkExtend]
@serial int
AS
INSERT [dbo].[tbl_AccountTrunk_Extend] (AccountSerial )
VALUES (@serial)




GO
/****** Object:  StoredProcedure [dbo].[pInsert_AccountTrunkItemCharge]    Script Date: 12/01/2026 16:57:20 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO





--
CREATE PROCEDURE [dbo].[pInsert_AccountTrunkItemCharge]
@AccountSerial int,
@TID tinyint,
@Money float,
@K int,
@D float,
@U int,
@R int
as
insert into [dbo].[tbl_AccountTrunkCharge] ( [AccountSerial], [TID], [Money], [K], [D], [U], [R] )
values( @AccountSerial, @TID, @Money, @K, @D, @U, @R )






GO
/****** Object:  StoredProcedure [dbo].[pInsert_AccountTrunkItemCharge_20070420]    Script Date: 12/01/2026 16:57:20 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO



CREATE PROCEDURE [dbo].[pInsert_AccountTrunkItemCharge_20070420]
@AccountSerial int,
@TID tinyint,
@Money float,
@K int,
@D float,
@U int,
@R int,
@T int
as
insert into [dbo].[tbl_AccountTrunkCharge] ( [AccountSerial], [TID], [Money], [K], [D], [U], [R], [T] )
values( @AccountSerial, @TID, @Money, @K, @D, @U, @R, @T )







GO
/****** Object:  StoredProcedure [dbo].[pInsert_AccountTrunkItemCharge_20071024]    Script Date: 12/01/2026 16:57:20 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO

CREATE PROCEDURE [dbo].[pInsert_AccountTrunkItemCharge_20071024]

@AccountSerial int,

@TID tinyint,

@Money float,

@K int,

@D bigint,

@U int,

@R int,

@S bigint,

@T int

as

insert into [dbo].[tbl_AccountTrunkCharge] ( [AccountSerial], [TID], [Money], [K], [D], [U], [R], [S], [T] )

values( @AccountSerial, @TID, @Money, @K, @D, @U, @R, @S, @T )





GO
/****** Object:  StoredProcedure [dbo].[pinsert_aminepersonal_inven]    Script Date: 12/01/2026 16:57:20 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO








create procedure [dbo].[pinsert_aminepersonal_inven]
@nAvatorSerial int
as
insert [dbo].[tbl_aminepersonal_inven] (avatorserial) values (@nAvatorSerial)







GO
/****** Object:  StoredProcedure [dbo].[pInsert_AnimusData]    Script Date: 12/01/2026 16:57:20 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO





--
create procedure [dbo].[pInsert_AnimusData]
@serial int,
@data0 float,
@data1 float,
@data2 float,
@data3 float,
@data4 float,
@data5 float
as
insert into [dbo].[tbl_animusdata] ( Serial, Data0, Data1, Data2, Data3, Data4, Data5 ) values ( @serial, @data0, @data1, @data2, @data3, @data4, @data5 )






GO
/****** Object:  StoredProcedure [dbo].[pInsert_AnimusLog]    Script Date: 12/01/2026 16:57:20 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO





--
create procedure [dbo].[pInsert_AnimusLog]
@serial int,
@name varchar(17),
@did tinyint,
@orgval float,
@chgval float
as
insert into [dbo].[tbl_animuslog] ( Serial, Name, DID, OrgValue, ChangeValue ) values ( @serial, @name, @did, @orgval, @chgval )






GO
/****** Object:  StoredProcedure [dbo].[pinsert_automine_newowner]    Script Date: 12/01/2026 16:57:20 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO







create procedure [dbo].[pinsert_automine_newowner]
@ntype tinyint,
@nrace tinyint,
@nguildserial int
as
insert [dbo].[tbl_automine_inven] (collisiontype, race, guildserial)values(@ntype, @nrace, @nguildserial)







GO
/****** Object:  StoredProcedure [dbo].[pInsert_Base]    Script Date: 12/01/2026 16:57:20 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO





/****** 개체: 저장 프로시저 dbo.pInsert_Avator_Base    스크립트 날짜: 2003-04-10 오후 9:05:29 ******/
CREATE PROCEDURE [dbo].[pInsert_Base]
@name NVARCHAR(16), 
@class VARCHAR(17),
@accountserial int, 
@account VARCHAR(17),
@slot int, 
@race int, 
@baseshape int
AS

INSERT [dbo].[tbl_base]
(
Name, AccountSerial, Account, Slot, Race, Class, BaseShape
)
VALUES 
(
@name, @accountserial, @account, @slot, @race, @class, @baseshape
)






GO
/****** Object:  StoredProcedure [dbo].[pInsert_Buddy]    Script Date: 12/01/2026 16:57:20 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO





--버디
CREATE PROCEDURE [dbo].[pInsert_Buddy]
@serial int
AS
INSERT [dbo].[tbl_Buddy] (Serial) VALUES (@serial)






GO
/****** Object:  StoredProcedure [dbo].[pInsert_CashLimSale]    Script Date: 12/01/2026 16:57:20 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
/*  
   작성자 : 이지혜   
   작성일 : 2008-07-23  
   내용    : pInsert_CashLimSale 프로시져 (캐시샵 한정판매)  
*/  
  
CREATE  PROCEDURE [dbo].[pInsert_CashLimSale]  
@dck tinyint  
AS  
INSERT [dbo].[tbl_cash_limsale] (DCK )  VALUES (@dck)



GO
/****** Object:  StoredProcedure [dbo].[pInsert_ClassLogAfterInitClass]    Script Date: 12/01/2026 16:57:20 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO





--
CREATE PROCEDURE [dbo].[pInsert_ClassLogAfterInitClass]
@Serial int,
@Type bit,
@PrevClass char(4),
@NextClass char(4),
@ClassInitCnt int,
@LastClassGrade tinyint,
@LogDate datetime
 AS 
INSERT INTO [dbo].[tbl_ClassLog_AfterInitClass]
( 
[Serial], [Type], [PrevClass], [NextClass], [ClassInitCnt], [LastClassGrade], [LogDate]
)
VALUES 
( 
@Serial, @Type, @PrevClass, @NextClass, @ClassInitCnt, @LastClassGrade, @LogDate 
)






GO
/****** Object:  StoredProcedure [dbo].[pInsert_CombineEx_Result]    Script Date: 12/01/2026 16:57:20 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO





--
CREATE PROCEDURE [dbo].[pInsert_CombineEx_Result]
@serial int
AS
INSERT [dbo].[tbl_itemcombine_ex_result] (Serial )
VALUES (@serial)






GO
/****** Object:  StoredProcedure [dbo].[pInsert_DefaultRecordWeeklyGuildPVPPointSum]    Script Date: 12/01/2026 16:57:20 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO





--
create procedure [dbo].[pInsert_DefaultRecordWeeklyGuildPVPPointSum]
as
insert into [dbo].[tbl_WeeklyGuildPVPPointSum] ( serial )
( select serial from [dbo].[tbl_guild] where not exists ( select * from [dbo].[tbl_WeeklyGuildPVPPointSum] where serial = [dbo].[tbl_guild].serial )
and dck = 0 )






GO
/****** Object:  StoredProcedure [dbo].[pInsert_defaultutgroupiteminfo]    Script Date: 12/01/2026 16:57:20 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO






create procedure [dbo].[pInsert_defaultutgroupiteminfo]
as
insert into [dbo].[tbl_utgroupiteminfo] ( item1, item2, item3, item4, item5, item6 ) values ( 0, 0, 0, 0, 0, 0 )







GO
/****** Object:  StoredProcedure [dbo].[pInsert_defaultutresultinfo]    Script Date: 12/01/2026 16:57:20 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO






create procedure [dbo].[pInsert_defaultutresultinfo]
@type tinyint,
@serial int,
@regdate datetime
as
insert into [dbo].[tbl_utresultinfo] ( type, serial, state, resultdate, buyer, tax ) values ( @type, @serial, 0, @regdate, 0, 0  )







GO
/****** Object:  StoredProcedure [dbo].[pInsert_defaultutsellinfo]    Script Date: 12/01/2026 16:57:20 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO






create procedure [dbo].[pInsert_defaultutsellinfo]
@type tinyint,
@serial int,
@regdate datetime
as
insert into [dbo].[tbl_utsellinfo] ( type, serial, race, regdate, owner, price, sellturm ) values ( @type, @serial, 255, @regdate, 0, 0, 0 )







GO
/****** Object:  StoredProcedure [dbo].[pInsert_defaultutsingleiteminfo]    Script Date: 12/01/2026 16:57:20 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO







create procedure [dbo].[pInsert_defaultutsingleiteminfo]
as
insert into [dbo].[tbl_utsingleiteminfo] ( inveninx, k, d, u, lv, grade, class1,  class2, class3 ) values ( 0xff, 0xffffffff, 0, 0x0fffffff, 0, 0, 0xff, 0xff, 0xff )







GO
/****** Object:  StoredProcedure [dbo].[pInsert_Economy_History]    Script Date: 12/01/2026 16:57:20 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO





--
CREATE PROCEDURE [dbo].[pInsert_Economy_History]
@nDate int,
@fBDalant float,
@fBGold float,
@fCDalant float,
@fCGold float,
@fADalant float,
@fAGold float,
@nManageValue int,

@fBMineOre1 float,
@fCMineOre1 float,
@fAMineOre1 float,
@fBMineOre2 float,
@fCMineOre2 float,
@fAMineOre2 float,
@fBMineOre3 float,
@fCMineOre3 float,
@fAMineOre3 float,
@fBCutOre1 float,
@fCCutOre1 float,
@fACutOre1 float,
@fBCutOre2 float,
@fCCutOre2 float,
@fACutOre2 float,
@fBCutOre3 float,
@fCCutOre3 float,
@fACutOre3 float
AS
insert into 
[dbo].[tbl_economy_history]
(
Serial, B_Dalant, B_Gold, C_Dalant, C_Gold, A_Dalant, A_Gold, ManageValue,
B_MineOre1, C_MineOre1, A_MineOre1,
B_MineOre2, C_MineOre2, A_MineOre2,
B_MineOre3, C_MineOre3, A_MineOre3,
B_CutOre1, C_CutOre1, A_CutOre1,
B_CutOre2, C_CutOre2, A_CutOre2,
B_CutOre3, C_CutOre3, A_CutOre3
) 
values
( 
@nDate, @fBDalant, @fBGold,@fCDalant, @fCGold, @fADalant, @fAGold, @nManageValue,
@fBMineOre1, @fCMineOre1, @fAMineOre1, 
@fBMineOre2, @fCMineOre2, @fAMineOre2, 
@fBMineOre3, @fCMineOre3, @fAMineOre3, 
@fBCutOre1, @fCCutOre1, @fACutOre1, 
@fBCutOre2, @fCCutOre2, @fACutOre2, 
@fBCutOre3, @fCCutOre3, @fACutOre3
)






GO
/****** Object:  StoredProcedure [dbo].[pInsert_General]    Script Date: 12/01/2026 16:57:20 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO





--
/****** Object:  Stored Procedure dbo.pInsert_Avator_Belt    Script Date: 2003-07-11 오전 10:04:53 ******/
CREATE PROCEDURE [dbo].[pInsert_General]
@serial int,
@map int
AS
INSERT [dbo].[tbl_general] (Serial, Map)
VALUES (@serial, @map)






GO
/****** Object:  StoredProcedure [dbo].[pInsert_GoldBoxItem]    Script Date: 12/01/2026 16:57:20 ******/
SET ANSI_NULLS OFF
GO
SET QUOTED_IDENTIFIER OFF
GO


CREATE  PROCEDURE [dbo].[pInsert_GoldBoxItem]
@dck tinyint
AS
INSERT [dbo].[tbl_GoldBoxItem] (DCK )
VALUES (@dck)




GO
/****** Object:  StoredProcedure [dbo].[pInsert_GreetingMsgRecord]    Script Date: 12/01/2026 16:57:20 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO




CREATE procedure [dbo].[pInsert_GreetingMsgRecord]
@useType tinyint,
@msg varchar(255)
as
insert into [dbo].[tbl_GreetMsg] ( useType, GMsg ) values ( @useType, @msg )







GO
/****** Object:  StoredProcedure [dbo].[pInsert_GreetingMsgRecord_071119]    Script Date: 12/01/2026 16:57:20 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO




CREATE procedure [dbo].[pInsert_GreetingMsgRecord_071119]
@useType tinyint,
@name varchar(17),
@msg varchar(255)  
as  
insert into [dbo].[tbl_GreetMsg] ( useType, Name, GMsg ) values ( @useType, @name, @msg )  





GO
/****** Object:  StoredProcedure [dbo].[pInsert_Guild]    Script Date: 12/01/2026 16:57:20 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO





--
CREATE PROCEDURE [dbo].[pInsert_Guild]
@id nvarchar(17),
@race int
AS
insert into [dbo].[tbl_guild] ( id, race ) values( @id, @race )

GO
/****** Object:  StoredProcedure [dbo].[pInsert_GuildBattleInfo]    Script Date: 12/01/2026 16:57:20 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO





--
create procedure [dbo].[pInsert_GuildBattleInfo]
@id int,
@p1serial int,
@p2serial int,
@mapid int,
@number tinyint
as    
insert into [dbo].[tbl_ReservedGuildBattleInfo]( [ID], [P1GuildSerial], [P2GuildSerial], [MapID], [Number] )    
values ( @id, @p1serial, @p2serial, @mapid, @number )






GO
/****** Object:  StoredProcedure [dbo].[pInsert_GuildBattleRank]    Script Date: 12/01/2026 16:57:20 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO





--
create procedure [dbo].[pInsert_GuildBattleRank]
@serial int
as  
insert into [dbo].[tbl_GuildBattleRank] ([Serial]) values (@serial)  






GO
/****** Object:  StoredProcedure [dbo].[pInsert_guildbattleresultlog]    Script Date: 12/01/2026 16:57:20 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO




create procedure [dbo].[pInsert_guildbattleresultlog]
@start smalldatetime,
@end smalldatetime,
@redserial int,
@redname varchar(17),
@blueserial int,
@bluename varchar(17),
@redscore int,
@bluescore int,
@redmaxjoinmembercnt int,
@bluemaxjoinmembercnt int,
@redgoalcnt int,
@bluegoalcnt int,
@redkillcnt int,
@bluekillcnt int,
@result tinyint,
@topgoalcharacserial int,
@topgoalcharacname varchar(17),
@topkillcharacserial int,
@topkillcharacname varchar(17),
@joinlimit tinyint,
@guildbattlecostgold int,
@mapcode varchar(12)
as
insert into [dbo].[tbl_guildbattleresullog] ( [start], [end]
, [redserial], [redname], [blueserial], [bluename], [redscore], [bluescore]
, [redmaxjoinmembercnt], [bluemaxjoinmembercnt]
, [redgoalcnt], [bluegoalcnt], [redkillcnt], [bluekillcnt], [result]
, [topgoalcharacserial], [topgoalcharacname], [topkillcharacserial], [topkillcharacname]
, [joinlimit], [guildbattlecostgold], [mapcode] )
values ( @start, @end
, @redserial, @redname, @blueserial, @bluename, @redscore, @bluescore
, @redmaxjoinmembercnt, @bluemaxjoinmembercnt
, @redgoalcnt, @bluegoalcnt, @redkillcnt, @bluekillcnt, @result
, @topgoalcharacserial, @topgoalcharacname, @topkillcharacserial, @topkillcharacname
, @joinlimit, @guildbattlecostgold, @mapcode )






GO
/****** Object:  StoredProcedure [dbo].[pInsert_GuildMoneyHistory_Log]    Script Date: 12/01/2026 16:57:20 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO





--
CREATE procedure [dbo].[pInsert_GuildMoneyHistory_Log]  
@GuildSerial int,  
@InoutDalant float,  
@InoutGold float,  
@ResultDalant float,  
@ResultGold float,  
@LogDate char(8),  
@AvatorSerial int,  
@AvatorName varchar(17)  
as  
insert   
[dbo].[tbl_GuildMoneyHistory_Log] ( GuildSerial,  InoutDalant, InoutGold, ResultDalant, ResultGold, LogDate, AvatorSerial, AvatorName )   
values( @GuildSerial, @InoutDalant, @InoutGold, @ResultDalant, @ResultGold, @LogDate, @AvatorSerial, @AvatorName )






GO
/****** Object:  StoredProcedure [dbo].[pInsert_HonorGuild]    Script Date: 12/01/2026 16:57:20 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO




/*
-- 인덱스 추가
-- 작성자 : 이희길
-- 작성일 : 2007-02-27(화)
-- 내용 : 프로시져에서 가장 많이 사용되는 필드에 인덱스 추가
CREATE INDEX [IX_tbl_honor_guild_Race] on [dbo].[tbl_honor_guild](Race)
CREATE INDEX [IX_tbl_honor_guild_IsNext] on [dbo].[tbl_honor_guild](IsNext)
*/



/*
   작성자 : 김태완
   작성일 : 2007-02-22 (목)
   내용 : 명예길드 데이터 입력
*/
CREATE PROCEDURE [dbo].[pInsert_HonorGuild]
@race	tinyint,
@gserial	int,
@tax	tinyint,
@isnext	tinyint
AS

INSERT INTO [dbo].[tbl_honor_guild] (race, guildserial, taxrate, isnext) VALUES (@race, @gserial, @tax, @isnext)






GO
/****** Object:  StoredProcedure [dbo].[pInsert_Inven]    Script Date: 12/01/2026 16:57:20 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO






/****** 개체: 저장 프로시저 dbo.pInsert_Avator_Inven    스크립트 날짜: 2003-04-10 오후 9:05:30 ******/
CREATE PROCEDURE [dbo].[pInsert_Inven]
@serial int
AS
INSERT [dbo].[tbl_inven] (Serial )
VALUES (@serial)






GO
/****** Object:  StoredProcedure [dbo].[pInsert_ItemCharge]    Script Date: 12/01/2026 16:57:20 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO





--
CREATE PROCEDURE [dbo].[pInsert_ItemCharge]
@avatorSerial int,
@ic_K int,
@ic_D int,
@ic_U int
AS
INSERT [dbo].[tbl_ItemCharge] ( nAvatorSerial, nItemCode_K, nItemCode_D, nItemCode_U ) 
VALUES( @avatorSerial, @ic_K, @ic_D, @ic_U )






GO
/****** Object:  StoredProcedure [dbo].[pInsert_ItemCharge_20070418]    Script Date: 12/01/2026 16:57:20 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO



/*
sp_helptext pInsert_ItemCharge
sp_help tbl_itemcharge
*/

CREATE PROCEDURE [dbo].[pInsert_ItemCharge_20070418]
@avatorSerial int,
@ic_K int,
@ic_D int,
@ic_U int,
@ic_RentTime int
AS
INSERT [dbo].[tbl_ItemCharge] ( nAvatorSerial, nItemCode_K, nItemCode_D, nItemCode_U, T ) 
VALUES( @avatorSerial, @ic_K, @ic_D, @ic_U, @ic_RentTime )







GO
/****** Object:  StoredProcedure [dbo].[pInsert_ItemCharge_20070806]    Script Date: 12/01/2026 16:57:20 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO


/*    
sp_helptext pInsert_ItemCharge    
sp_help tbl_itemcharge    
*/    
    
CREATE PROCEDURE [dbo].[pInsert_ItemCharge_20070806]    
@avatorSerial int,    
@ic_K int,    
@ic_D bigint,    
@ic_U int,    
@ic_RentTime int    
AS    
INSERT [dbo].[tbl_ItemCharge] ( nAvatorSerial, nItemCode_K, nItemCode_D, nItemCode_U, T )     
VALUES( @avatorSerial, @ic_K, @ic_D, @ic_U, @ic_RentTime )    






GO
/****** Object:  StoredProcedure [dbo].[pInsert_ItemChargeInGame]    Script Date: 12/01/2026 16:57:20 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO





--
CREATE procedure [dbo].[pInsert_ItemChargeInGame]
@serial int,  
@k int,  
@d int,  
@u int,
@type tinyint
as  
insert into [dbo].[tbl_ItemCharge] ( nAvatorSerial, nItemCode_K, nItemCode_D, nItemCode_U, Type ) values ( @serial, @k, @d, @u, @type )






GO
/****** Object:  StoredProcedure [dbo].[pInsert_Level_Log]    Script Date: 12/01/2026 16:57:20 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO






/*
레벨업 로그 추가
*/
CREATE PROCEDURE [dbo].[pInsert_Level_Log]
@serial int,
@level int,
@playMin int
AS
INSERT [dbo].[tbl_log_lv] (AvatorSerial, Lv, TotalMin )
VALUES (@serial, @level, @playMin)






GO
/****** Object:  StoredProcedure [dbo].[pInsert_NpcData]    Script Date: 12/01/2026 16:57:20 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO






--NPC데이터
CREATE PROCEDURE [dbo].[pInsert_NpcData]
@serial int
AS
INSERT [dbo].[tbl_NpcData] (Serial) VALUES (@serial)






GO
/****** Object:  StoredProcedure [dbo].[pInsert_NpcData2]    Script Date: 12/01/2026 16:57:20 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO






--NPC데이터 With Value
CREATE PROCEDURE [dbo].[pInsert_NpcData2]
@serial int,
@data0 int,
@data1 int,
@data2 int,
@data3 int,
@data4 int,
@data5 int
AS
INSERT tbl_NpcData(Serial,Npc0,Npc1,Npc2,Npc3,Npc4,Npc5) 
VALUES (@serial,@data0,@data1,@data2,@data3,@data4,@data5)






GO
/****** Object:  StoredProcedure [dbo].[pInsert_NpcLog]    Script Date: 12/01/2026 16:57:20 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO






CREATE PROCEDURE [dbo].[pInsert_NpcLog]
@CharSerial int,
@name varchar(17),
@vIndex int,
@oValue int,
@cValue int
AS
INSERT [dbo].[tbl_NpcLog] ( CharSerial, Name, ValueIndex, OrgValue, ChangeValue ) 
VALUES ( @CharSerial, @name, @vIndex, @oValue, @cValue   )






GO
/****** Object:  StoredProcedure [dbo].[pInsert_NpcQuest_History]    Script Date: 12/01/2026 16:57:20 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO




/*
   작성자 : 김태완
   작성일 : 2007-05-21 (월)
   내용 : NPC 퀘스트 완료 히스토리 디폴트 레코드 입력
*/
CREATE PROCEDURE [dbo].[pInsert_NpcQuest_History]
@serial int

AS

INSERT [dbo].[tbl_npc_quest_history] (Serial) VALUES (@serial)





GO
/****** Object:  StoredProcedure [dbo].[pInsert_OreCutting]    Script Date: 12/01/2026 16:57:20 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO




/*  
   작성자 : 정우철
   작성일 : 2007-12-05 (수)
   내용    : tbl_OreCutting Insert Procedure
*/  

create procedure [dbo].[pInsert_OreCutting]
@serial int

as

insert into [dbo].[tbl_OreCutting](Serial) values(@serial)






GO
/****** Object:  StoredProcedure [dbo].[pInsert_Patriarch_Candidate]    Script Date: 12/01/2026 16:57:20 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO




/*
-- 인덱스 추가
-- 작성자 : 이희길
-- 작성일 : 2007-02-27(화)
-- 내용 : 프로시져에서 사용되는 쿼리 중 참조가 가장 많이 되는 필드에 인덱스 추가
CREATE INDEX [IX_tbl_patriarch_candidate_eSerial] on [dbo].[tbl_patriarch_candidate](eSerial)
CREATE INDEX [IX_tbl_patriarch_candidate_ASerial] on [dbo].[tbl_patriarch_candidate](ASerial)
*/




/*
   작성자 : 김태완
   작성일 : 2007-02-09 (금)
   내용 : 족장선출 시스템 후보자 레코드 입력
*/
CREATE PROCEDURE [dbo].[pInsert_Patriarch_Candidate]
@eserial	int,
@race	tinyint,
@lv	tinyint,
@rank	int,
@pvppnt	float,
@aserial	int,
@aname	varchar(17),
@gserial	int,
@gname	varchar(17),
@class	tinyint,
@status	tinyint

AS

Insert into [dbo].[tbl_patriarch_candidate] (eSerial, Race, Lv, Rank, PvpPoint, ASerial, AName, GSerial, GName, ClassType, State, dtUpScore)
Values (@eserial, @race, @lv, @rank, @pvppnt, @aserial, @aname, @gserial, @gname, @class, @status, getdate() )






GO
/****** Object:  StoredProcedure [dbo].[pInsert_Patriarch_Candidate_070313]    Script Date: 12/01/2026 16:57:20 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO







/*
   작성자 : 김태완
   작성일 : 2007-03-13 (화)
   내용 : 족장선출 시스템 후보자 레코드 입력 수정 (등급 추가)
*/
CREATE PROCEDURE [dbo].[pInsert_Patriarch_Candidate_070313]
@eserial	int,
@race	tinyint,
@lv	tinyint,
@rank	int,
@grade	tinyint,
@pvppnt	float,
@aserial	int,
@aname	varchar(17),
@gserial	int,
@gname	varchar(17),
@class	tinyint,
@status	tinyint

AS

Insert into [dbo].[tbl_patriarch_candidate] (eSerial, Race, Lv, Rank, Grade, PvpPoint, ASerial, AName, GSerial, GName, ClassType, State, dtUpScore)
Values (@eserial, @race, @lv, @rank, @grade, @pvppnt, @aserial, @aname, @gserial, @gname, @class, @status, getdate() )






GO
/****** Object:  StoredProcedure [dbo].[pInsert_Patriarch_Elect]    Script Date: 12/01/2026 16:57:20 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO






/*
   작성자 : 김태완
   작성일 : 2007-02-09 (금)
   내용 : 족장선출 시스템 선출 레코드 입력
*/
CREATE PROCEDURE [dbo].[pInsert_Patriarch_Elect]
@wname varchar(32),
@ptype	tinyint

AS

INSERT INTO [dbo].[tbl_patriarch_elect] (worldname, proctype) VALUES (@wname, @ptype)







GO
/****** Object:  StoredProcedure [dbo].[pInsert_PatriarchComm]    Script Date: 12/01/2026 16:57:20 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO






/*
   작성자 : 김태완
   작성일 : 2007-02-08 (목)
   내용 : 무인거래기 족장 입금 수수료 데이터를 입력 한다.
*/
CREATE PROCEDURE [dbo].[pInsert_PatriarchComm]
@aserial int,
@dalant int,
@depdate char(8)
AS

INSERT INTO [dbo].[tbl_patriarch_comm] (AvatorSerial, Dalant, DepositDate) VALUES (@aserial, @dalant, @depdate)






GO
/****** Object:  StoredProcedure [dbo].[pInsert_PcbangItem]    Script Date: 12/01/2026 16:57:20 ******/
SET ANSI_NULLS OFF
GO
SET QUOTED_IDENTIFIER OFF
GO


/*
   작성자 : 안우태
   작성일 : 2008-02-17
   내용    : pInsert_PcbangItem 프로시져 (PC방 혜택 강화)
*/

CREATE PROCEDURE [dbo].[pInsert_PcbangItem]
@serial int
AS
INSERT [dbo].[tbl_pcbangitem] (Serial )
VALUES (@serial)




GO
/****** Object:  StoredProcedure [dbo].[pInsert_PostStorageRecord]    Script Date: 12/01/2026 16:57:20 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO



-- if exists (select * from dbo.sysobjects where id = object_id(N'[dbo].[pInsert_PostStorageRecord]') and OBJECTPROPERTY(id, N'IsProcedure') = 1)
-- drop procedure [dbo].[pInsert_PostStorageRecord]
-- GO
-- 
-- SET QUOTED_IDENTIFIER OFF 
-- GO
-- SET ANSI_NULLS ON 
-- GO


CREATE procedure [dbo].[pInsert_PostStorageRecord]
as
insert into [dbo].[tbl_PostStorage] ( postinx, owner, poststate, k, d, u, gold, err ) values ( 0xff, 0, 0, 0xffffffff, 0, 0x0fffffff, 0, 0 )






GO
/****** Object:  StoredProcedure [dbo].[pInsert_potiondelay]    Script Date: 12/01/2026 16:57:20 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO


/*  
   작성자 : 정우철
   작성일 : 2007-08-16 (목)
   내용    : pInsert_potiondelay 프로시져 생성
*/  

create procedure [dbo].[pInsert_potiondelay]
@serial int

as

insert into [dbo].[tbl_potion_delay](Serial) values(@serial)






GO
/****** Object:  StoredProcedure [dbo].[pInsert_pplirecord]    Script Date: 12/01/2026 16:57:20 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO






create procedure [dbo].[pInsert_pplirecord]
@serial int
as
insert [dbo].[tbl_pvppointlimitinfo] ([serial]) values (@serial)







GO
/****** Object:  StoredProcedure [dbo].[pInsert_PrimiumPlayTime]    Script Date: 12/01/2026 16:57:20 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER OFF
GO




CREATE  procedure [dbo].[pInsert_PrimiumPlayTime]
@serial int
as
insert into [dbo].[tbl_PrimiumPlayTime] (AccountSerial ) 
VALUES (@serial)





GO
/****** Object:  StoredProcedure [dbo].[pInsert_Punishment]    Script Date: 12/01/2026 16:57:20 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO




/*
-- 인덱스 추가
-- 작성자 : 이희길
-- 작성일 : 2007-02-27(화)
-- 내용 : 프로시져에서 가장 많이 사용되는 필드에 인덱스 추가
CREATE CLUSTERED INDEX [IX_tbl_Punishment_ASerial] on [dbo].[tbl_Punishment](ASerial)
CREATE INDEX [IX_tbl_Punishment_ESerial] on [dbo].[tbl_Punishment](ESerial)
*/


/*
캐릭터의 족장제재 정보를 입력한다
*/
CREATE PROCEDURE [dbo].[pInsert_Punishment]
@ASerial		int,	---캐릭터 시리얼
@ESerial		int,	---족장임기 시리얼
@Type		tinyint,	---제재 종류(0:채팅, 1:현상범, 2:파티)
@Value		int	---제재 받은 시간 (분)
AS
INSERT INTO [dbo].[tbl_punishment] VALUES (@ASerial, @ESerial, @Type, @Value)






GO
/****** Object:  StoredProcedure [dbo].[pInsert_PvpOrderView]    Script Date: 12/01/2026 16:57:20 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO



create procedure [dbo].[pInsert_PvpOrderView]
@serial int
as
insert into [dbo].[tbl_pvporderview] (serial, pvppoint ) 
                          select serial, pvppoint from [dbo].[tbl_general] where serial = @serial




SET QUOTED_IDENTIFIER OFF 




GO
/****** Object:  StoredProcedure [dbo].[pInsert_Quest]    Script Date: 12/01/2026 16:57:20 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO









CREATE PROCEDURE [dbo].[pInsert_Quest]
@serial int
AS
INSERT [dbo].[tbl_quest] (Serial) VALUES (@serial)







GO
/****** Object:  StoredProcedure [dbo].[pInsert_RaceBattleLog]    Script Date: 12/01/2026 16:57:20 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO





/*
   작성자 : 김태완
   작성일 : 2007-03-02 (금)
   내용 : 종족전 결과 레코드 입력 프로시져
*/
CREATE PROCEDURE [dbo].[pInsert_RaceBattleLog]
@wname	varchar(32),
@szdate		char(8),
@nth		tinyint,
@endtime	int,
@wrace		tinyint,
@lrace		tinyint
AS

INSERT INTO [dbo].[tbl_racebattle_log] (worldname, szdate, nth, endtime, winrace, loserace) VALUES (@wname, @szdate, @nth, @endtime, @wrace, @lrace)






GO
/****** Object:  StoredProcedure [dbo].[pInsert_RaceBattleLog_070427]    Script Date: 12/01/2026 16:57:20 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO



/*
   작성자 : 김태완
   작성일 : 2007-03-02 (금)
   내용 : 종족전 결과 레코드 입력

   수정자 : 정우철
   수정일 : 2007-04-27
   수정내용 : 각 족장 시리얼 추가
*/

CREATE PROCEDURE [dbo].[pInsert_RaceBattleLog_070427]
@wname	varchar(32),
@szdate	char(8),
@nth	tinyint,
@endtime	int,
@wrace	tinyint,
@lrace	tinyint,
@boss0 int,
@boss1 int,
@boss2 int
AS

INSERT INTO [dbo].[tbl_racebattle_log] (worldname, szdate, nth, endtime, winrace, loserace,bossserial0,bossserial1,bossserial2) VALUES (@wname, @szdate, @nth, @endtime, @wrace, @lrace, @boss0, @boss1, @boss2)






GO
/****** Object:  StoredProcedure [dbo].[pInsert_RemainOre_Log]    Script Date: 12/01/2026 16:57:20 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER OFF
GO
/*
2008-12-05
АЫѕчАЪ : АУјєИс
їАѕо ГК±вИ­ ·О±Ч ГЯ°Ў
*/
CREATE PROCEDURE [dbo].[pInsert_RemainOre_Log]
@LogType tinyint,
@logDate varchar(17),
@livenum int,
@remain int,
@totalAmount int
AS
INSERT [dbo].[tbl_RemainOre_Log] (LogType,logDate, livenum, remain, totalAmount )
VALUES (@LogType,@logDate, @livenum, @remain, @totalAmount)


GO
/****** Object:  StoredProcedure [dbo].[pInsert_RenamePotionLog]    Script Date: 12/01/2026 16:57:20 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO



CREATE procedure [dbo].[pInsert_RenamePotionLog]

@Serial int,

@OldName varchar(17),

@NewName varchar(17)

as

insert into [dbo].[tbl_RenamePotion_Log] ( [Serial], [OldName], [NewName] )

values ( @Serial, @OldName, @NewName )






GO
/****** Object:  StoredProcedure [dbo].[pInsert_settlementownerlog]    Script Date: 12/01/2026 16:57:20 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO







create procedure [dbo].[pInsert_settlementownerlog]
@owndate smalldatetime,
@nth tinyint,
@race tinyint,
@guildserial int,
@guildname varchar(17),
@pvprank int,
@guildgrade tinyint,
@killpvppointsum float,
@battlepvppoinsum float,
@guildmembersumlv int
as
insert into [dbo].[tbl_settlementownerlog] ([owndate], [nth], [race], [guildserial], [guildname], [pvprank], [guildgrade], [killpvppointsum], [battlepvppoinsum], [guildmembersumlv] )
values ( @owndate, @nth, @race, @guildserial, @guildname, @pvprank, @guildgrade, @killpvppointsum, @battlepvppoinsum, @guildmembersumlv )







GO
/****** Object:  StoredProcedure [dbo].[pInsert_SFDelay]    Script Date: 12/01/2026 16:57:20 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO





/*
   작성자 : 김태완
   작성일 : 2007-04-16 (월)
   내용 : 스킬,포스 딜레이 데이터 입력 프로시져
*/
CREATE PROCEDURE [dbo].[pInsert_SFDelay]
@aserial	int,
@effect	binary(130)

AS

Insert into tbl_sf_delay ( aserial, effect ) values ( @aserial, @effect )






GO
/****** Object:  StoredProcedure [dbo].[pInsert_Start_Npc_Quest_History_Type1]    Script Date: 12/01/2026 16:57:20 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO

/****** 개체: 저장 프로시저 NPC 시작 퀘스트에 대한 수행 여부 히스토리를 입력 한다. ******/ 
CREATE  procedure [dbo].[pInsert_Start_Npc_Quest_History_Type1]
@serial int,
@code varchar(64),
@level  tinyint,
@startdate datetime,
@endtime bigint

as
insert into [dbo].[tbl_start_npc_quest_history] ( [serial], [code], [level], [startdate], [endtime]) values ( @serial, @code, @level, @startdate, @endtime )




GO
/****** Object:  StoredProcedure [dbo].[pInsert_StoreLimitItemRecord]    Script Date: 12/01/2026 16:57:20 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO



-- if exists (select * from dbo.sysobjects where id = object_id(N'[dbo].[pInsert_StoreLimitItemRecord]') and OBJECTPROPERTY(id, N'IsProcedure') = 1)
-- drop procedure [dbo].[pInsert_PostStorageRecord]
-- GO
-- 
-- SET QUOTED_IDENTIFIER OFF 
-- GO
-- SET ANSI_NULLS ON 
-- GO


CREATE procedure [dbo].[pInsert_StoreLimitItemRecord]
as
insert into [dbo].[tbl_StoreLimitItem] ( mapinx, storeinx, dck ) values ( 0, 0, 1 )






GO
/****** Object:  StoredProcedure [dbo].[pInsert_StoreLimitItemRecord_061212]    Script Date: 12/01/2026 16:57:20 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO



create procedure [dbo].[pInsert_StoreLimitItemRecord_061212]
as
insert into [dbo].[tbl_StoreLimitItem_061212] ( typeserial, storeinx, dck ) values ( 0, 0, 1 )






GO
/****** Object:  StoredProcedure [dbo].[pInsert_supplement]    Script Date: 12/01/2026 16:57:20 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO


/*  
   작성자 : 정우철
   작성일 : 2007-07-26 (목)  
   내용    : tbl_supplement 레코드 삽입
*/  

CREATE PROCEDURE [dbo].[pInsert_supplement]
@serial  int   --아바타 시리얼

AS

INSERT [dbo].[tbl_supplement] (Serial)
VALUES (@serial)






GO
/****** Object:  StoredProcedure [dbo].[pInsert_Unit]    Script Date: 12/01/2026 16:57:20 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO








CREATE PROCEDURE [dbo].[pInsert_Unit]
@serial int
AS
INSERT [dbo].[tbl_unit] (Serial )
VALUES (@serial)







GO
/****** Object:  StoredProcedure [dbo].[pInsert_UnitData]    Script Date: 12/01/2026 16:57:20 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO







create procedure [dbo].[pInsert_UnitData]
@serial int,
@data0 float,
@data1 float
as
insert into [dbo].[tbl_unitdata] ( Serial, Data0, Data1 ) values ( @serial, @data0, @data1 )







GO
/****** Object:  StoredProcedure [dbo].[pInsert_UnitLog]    Script Date: 12/01/2026 16:57:20 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO






create procedure [dbo].[pInsert_UnitLog]  
@serial int,  
@name varchar(17),
@did tinyint,  
@oldval float,  
@chgval float  
as  
insert into [dbo].[tbl_unitlog] ( Serial, Name, DID, OrgValue, ChangeValue ) values ( @serial, @name, @did, @oldval, @chgval )







GO
/****** Object:  StoredProcedure [dbo].[pInsert_UserInterface]    Script Date: 12/01/2026 16:57:20 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO








CREATE PROCEDURE [dbo].[pInsert_UserInterface]
@serial int
AS
INSERT [dbo].[tbl_userinterface] (Serial) VALUES (@serial)







GO
/****** Object:  StoredProcedure [dbo].[pInsert_UserNum_Log]    Script Date: 12/01/2026 16:57:20 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO






/*
동접자 로그를 추가한다
*/
CREATE PROCEDURE [dbo].[pInsert_UserNum_Log]
@nAvgNum int,
@nMaxNum int
AS
INSERT INTO [dbo].[tbl_log_usernum] (nAverageUser, nMaxUser) VALUES (@nAvgNum, @nMaxNum)







GO
/****** Object:  StoredProcedure [dbo].[pInsert_utsingledefaultrecord]    Script Date: 12/01/2026 16:57:20 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO


/*
 무인거래기 최초 적용시 기본 레코드 삽입용 프로시져
 최초 적용시만 사용함
*/

create procedure [dbo].[pInsert_utsingledefaultrecord]
@regdate datetime
as
set nocount on
declare @serial int
exec pInsert_defaultutsingleiteminfo
select top 1 @serial = serial from [dbo].[tbl_utsingleiteminfo] order by serial desc
if @serial is not null and @@rowcount = 1
begin
	begin tran
	exec pInsert_defaultutsellinfo 0, @serial, @regdate
	if @@rowcount <> 1
		begin
			rollback tran
			print 'rollback tran pInsert_defaultutsellinfo 0, '+ cast( @serial as varchar )
			return
		end
	else
		begin
			exec pInsert_defaultutresultinfo 0, @serial, @regdate
			if @@rowcount <> 1
				begin
					rollback tran
					print 'rollback tran pInsert_defaultutresultinfo 0, ' + cast( @serial as varchar )
					return
				end 
		end
	commit tran
end
set nocount off





GO
/****** Object:  StoredProcedure [dbo].[pInsert_WeeklyGuildPVPPointSum]    Script Date: 12/01/2026 16:57:20 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO






create procedure [dbo].[pInsert_WeeklyGuildPVPPointSum]
@serial int
as
insert into [dbo].[tbl_WeeklyGuildPVPPointSum] (serial) values ( @serial )







GO
/****** Object:  StoredProcedure [dbo].[pLock_Avator]    Script Date: 12/01/2026 16:57:20 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO









CREATE PROCEDURE [dbo].[pLock_Avator]
@lock int,
@serial int
AS
UPDATE [dbo].[tbl_base] SET Lock = @lock  WHERE Serial = @serial







GO
/****** Object:  StoredProcedure [dbo].[pMax_Serial]    Script Date: 12/01/2026 16:57:20 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO









CREATE PROCEDURE [dbo].[pMax_Serial]
AS
SELECT MAX(Serial) As MaxSerial
FROM [dbo].[tbl_base]







GO
/****** Object:  StoredProcedure [dbo].[prc_delete_pcbangitem]    Script Date: 12/01/2026 16:57:20 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
create procedure [dbo].[prc_delete_pcbangitem]
@ikey int
as

declare @Q nvarchar(500)
declare @i int
declare @Tc int
declare @Ti int

set @Tc	= convert( int, @ikey & 0x0000ff00 ) / 0xff
set @Ti	= convert( int, @ikey & 0xffff0000 ) / 0xffff

set @i  = 0

while @i < 8
begin

if @Tc = @i
begin
set @Q = ''
set @Q = @Q + 'update tbl_base set ek' + convert(varchar, @i) + '= -1 , eu' + convert(varchar, @i) + ' = convert(int, 0x0fffffff) '
set @Q = @Q + 'from tbl_base where ek' + convert(varchar, @i) + ' = ' + convert(varchar, @Ti)
-- print @Q
exec sp_executesql @Q
end

set @i = @i +1
end

set @i  = 0

while @i < 6
begin
set @Q = ''
set @Q = @Q + 'update tbl_general set ek' + convert(varchar, @i) + '= -1 '
set @Q = @Q + 'from tbl_general where (convert( int, ek' + convert(varchar,@i ) + ' & 0x0000ff00 ) / 0xff ) = '
set @Q = @Q + convert(varchar, @Tc)+ ' and '
set @Q = @Q + '(convert( int, ek' + convert(varchar,@i ) + '  & 0xffff0000 ) / 0xffff) = '
set @Q = @Q + convert(varchar, @Ti)
-- print @Q
exec sp_executesql @Q
set @i = @i +1
end


set @i  = 0

while @i < 100
begin
set @Q = ''
set @Q = @Q + 'update tbl_inven set k' + convert(varchar,@i ) + ' = -1, d' + convert(varchar,@i ) + ' = 0 , '
set @Q = @Q + 'u' + convert(varchar,@i ) + ' = convert(int, 0x0fffffff) from tbl_inven ' 
set @Q = @Q + 'where (convert( int, k' + convert(varchar,@i ) + ' & 0x0000ff00 ) / 0xff ) = '
set @Q = @Q + convert(varchar, @Tc)+ ' and '
set @Q = @Q + '(convert( int, k' + convert(varchar,@i ) + '  & 0xffff0000 ) / 0xffff) = '
set @Q = @Q + convert(varchar, @Ti)
-- print @Q
exec sp_executesql @Q
set @i = @i +1
end




GO
/****** Object:  StoredProcedure [dbo].[prc_delete_pcbangitem_by_upcode_for_unify]    Script Date: 12/01/2026 16:57:20 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE procedure [dbo].[prc_delete_pcbangitem_by_upcode_for_unify]
@ikey int
as
SET NOCOUNT ON
declare @Q nvarchar(500)
declare @i int
declare @Tc int
declare @Ti int

set @Tc	= convert( int, @ikey & 0x0000ff00 ) / 0xff
set @Ti	= convert( int, @ikey & 0xffff0000 ) / 0xffff

set @i  = 0

while @i < 8
begin

if @Tc = @i
begin
set @Q = ''
set @Q = @Q + 'update tbl_base set ek' + convert(varchar, @i) + '= -1 , eu' + convert(varchar, @i) + ' = convert(int, 0x0fffffff) '
set @Q = @Q + 'from tbl_base where ek' + convert(varchar, @i) + ' = ' + convert(varchar, @Ti) + ' and eu'+ convert(varchar, @i) + ' not in (1609913685,1878349141,2146784597,1867863381,2136298837,1968526677)'
-- print @Q
exec sp_executesql @Q
end

set @i = @i +1
end

set @i  = 0

while @i < 100
begin
set @Q = ''
set @Q = @Q + 'update tbl_inven set k' + convert(varchar,@i ) + ' = -1, d' + convert(varchar,@i ) + ' = 0 , '
set @Q = @Q + 'u' + convert(varchar,@i ) + ' = convert(int, 0x0fffffff) from tbl_inven ' 
set @Q = @Q + 'where (convert( int, k' + convert(varchar,@i ) + ' & 0x0000ff00 ) / 0xff ) = '
set @Q = @Q + convert(varchar, @Tc)+ ' and '
set @Q = @Q + '(convert( int, k' + convert(varchar,@i ) + '  & 0xffff0000 ) / 0xffff) = '
set @Q = @Q + convert(varchar, @Ti) + ' and u'+ convert(varchar, @i) + ' not in (1609913685,1878349141,2146784597,1867863381,2136298837,1968526677)'
-- print @Q
exec sp_executesql @Q
set @i = @i +1
end


set @Q = ''
set @Q = @Q + 'delete tbl_accounttrunkcharge where (convert( int, k & 0x0000ff00 ) / 0xff ) = ' + convert(varchar, @Tc) 
set @Q = @Q + ' and (convert( int, k & 0xffff0000 ) / 0xffff) = '+ convert(varchar, @Ti) + ' and u not in '
set @Q = @Q + '(1609913685,1878349141,2146784597,1867863381,2136298837,1968526677)'
-- print @Q
exec sp_executesql @Q
SET NOCOUNT OFF

/****** 개체:  StoredProcedure [dbo].[pCopy_MakeAvator]    스크립트 날짜: 03/03/2009 09:53:12 ******/
SET ANSI_NULLS ON
GO
/****** Object:  StoredProcedure [dbo].[prc_delete_pcbangitem_u]    Script Date: 12/01/2026 16:57:20 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO

-- 아이템 삭제 프로시저

create procedure [dbo].[prc_delete_pcbangitem_u]
@ikey int
as

declare @Q nvarchar(500)
declare @i int
declare @Tc int
declare @Ti int

set @Tc	= convert( int, @ikey & 0x0000ff00 ) / 0xff
set @Ti	= convert( int, @ikey & 0xffff0000 ) / 0xffff

set @i  = 0

while @i < 8
begin

if @Tc = @i
begin
set @Q = ''
set @Q = @Q + 'update tbl_base set ek' + convert(varchar, @i) + '= -1 , eu' + convert(varchar, @i) + ' = convert(int, 0x0fffffff) '
set @Q = @Q + 'from tbl_base where ek' + convert(varchar, @i) + ' = ' + convert(varchar, @Ti) + ' and eu'+ convert(varchar, @i) + ' not in (1609913685,1878349141,2146784597,1867863381,2136298837,1968526677)'
-- print @Q
exec sp_executesql @Q
end

set @i = @i +1
end

set @i  = 0

while @i < 100
begin
set @Q = ''
set @Q = @Q + 'update tbl_inven set k' + convert(varchar,@i ) + ' = -1, d' + convert(varchar,@i ) + ' = 0 , '
set @Q = @Q + 'u' + convert(varchar,@i ) + ' = convert(int, 0x0fffffff) from tbl_inven ' 
set @Q = @Q + 'where (convert( int, k' + convert(varchar,@i ) + ' & 0x0000ff00 ) / 0xff ) = '
set @Q = @Q + convert(varchar, @Tc)+ ' and '
set @Q = @Q + '(convert( int, k' + convert(varchar,@i ) + '  & 0xffff0000 ) / 0xffff) = '
set @Q = @Q + convert(varchar, @Ti) + ' and u'+ convert(varchar, @i) + ' not in (1609913685,1878349141,2146784597,1867863381,2136298837,1968526677)'
-- print @Q
exec sp_executesql @Q
set @i = @i +1
end


set @i  = 0

while @i < 100
begin
set @Q = ''
set @Q = @Q + 'update tbl_accounttrunk set k' + convert(varchar,@i ) + ' = -1, d' + convert(varchar,@i ) + ' = 0 , '
set @Q = @Q + 'u' + convert(varchar,@i ) + ' = convert(int, 0x0fffffff) from tbl_accounttrunk ' 
set @Q = @Q + 'where (convert( int, k' + convert(varchar,@i ) + ' & 0x0000ff00 ) / 0xff ) = '
set @Q = @Q + convert(varchar, @Tc)+ ' and '
set @Q = @Q + '(convert( int, k' + convert(varchar,@i ) + '  & 0xffff0000 ) / 0xffff) = '
set @Q = @Q + convert(varchar, @Ti) + ' and u'+ convert(varchar, @i) + ' not in (1609913685,1878349141,2146784597,1867863381,2136298837,1968526677)'
-- print @Q
exec sp_executesql @Q
set @i = @i +1
end

set @i  = 0

while @i < 40
begin
set @Q = ''
set @Q = @Q + 'update tbl_accounttrunk_extend set k' + convert(varchar,@i ) + ' = -1, d' + convert(varchar,@i ) + ' = 0 , '
set @Q = @Q + 'u' + convert(varchar,@i ) + ' = convert(int, 0x0fffffff) from tbl_accounttrunk_extend ' 
set @Q = @Q + 'where (convert( int, k' + convert(varchar,@i ) + ' & 0x0000ff00 ) / 0xff ) = '
set @Q = @Q + convert(varchar, @Tc)+ ' and '
set @Q = @Q + '(convert( int, k' + convert(varchar,@i ) + '  & 0xffff0000 ) / 0xffff) = '
set @Q = @Q + convert(varchar, @Ti) + ' and u'+ convert(varchar, @i) + ' not in (1609913685,1878349141,2146784597,1867863381,2136298837,1968526677)'
-- print @Q
exec sp_executesql @Q
set @i = @i +1
end



GO
/****** Object:  StoredProcedure [dbo].[pRebirth_Base]    Script Date: 12/01/2026 16:57:20 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO








/*
삭제 대기상태의 캐릭터 레코드를 살림
*/
CREATE PROCEDURE [dbo].[pRebirth_Base]
@serial int,
@name varchar(16)
AS
UPDATE [dbo].[tbl_base] SET DCK = 0, name=@name, DeleteName='*' 
WHERE Serial = @serial







GO
/****** Object:  StoredProcedure [dbo].[pSearch_AccItem]    Script Date: 12/01/2026 16:57:20 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO




-- 장신구 검색 프로시져 : tbl_general, tbl_base 조인
CREATE PROCEDURE [dbo].[pSearch_AccItem]
@iTopCount int,
@iCheckSerial int
AS
-- Begin_
	declare @nQuery nvarchar(400)
	Set @nQuery = N''
	Set @nQuery = @nQuery + N'Select top ' + convert(nvarchar(3), @iTopCount ) + N' '
	Set @nQuery = @nQuery + N'b.serial, b.name, b.account, b.accountserial, b.lv, '
	Set @nQuery = @nQuery + N'g.ek0, g.ed0, '
	Set @nQuery = @nQuery + N'g.ek1, g.ed1, '
	Set @nQuery = @nQuery + N'g.ek2, g.ed2, '
	Set @nQuery = @nQuery + N'g.ek3, g.ed3, '
	Set @nQuery = @nQuery + N'g.ek4, g.ed4, '
	Set @nQuery = @nQuery + N'g.ek5, g.ed5 '
	Set @nQuery = @nQuery + N'From tbl_general as g inner join tbl_base as b on g.serial = b.serial '
	Set @nQuery = @nQuery + N'Where b.dck = 0 and b.accountserial < 2000000000 and b.serial >  ' + convert(nvarchar(10), @iCheckSerial) + N' order by b.serial asc'
--	Set @nQuery = @nQuery + N'Where b.dck = 0 and b.serial >  ' + convert(nvarchar(10), @iCheckSerial) + N' order by b.serial asc'

	exec sp_executesql @nQuery
-- End_






GO
/****** Object:  StoredProcedure [dbo].[pSearch_EquipItem]    Script Date: 12/01/2026 16:57:20 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO





-- 착용장비 검색 프로시져 : tbl_base
CREATE PROCEDURE [dbo].[pSearch_EquipItem]
@iTopCount int,
@iCheckSerial int
AS
-- Begin_
	declare @nQuery nvarchar(512)
	Set @nQuery = N''
	Set @nQuery = @nQuery + N'Select top ' + convert(nvarchar(3), @iTopCount) + N' '
	Set @nQuery = @nQuery + N'serial, name, account, accountserial, lv, '
	Set @nQuery = @nQuery + N'ek0, eu0, '
	Set @nQuery = @nQuery + N'ek1, eu1, '
	Set @nQuery = @nQuery + N'ek2, eu2, '
	Set @nQuery = @nQuery + N'ek3, eu3, '
	Set @nQuery = @nQuery + N'ek4, eu4, '
	Set @nQuery = @nQuery + N'ek5, eu5, '
	Set @nQuery = @nQuery + N'ek6, eu6, '
	Set @nQuery = @nQuery + N'ek7, eu7 '
	Set @nQuery = @nQuery + N'From tbl_base '
	Set @nQuery = @nQuery + N'Where dck = 0 and accountserial < 2000000000 and serial >  ' + convert(nvarchar(10), @iCheckSerial) + N' order by serial asc'
--	Set @nQuery = @nQuery + N'Where dck = 0 and serial >  ' + convert(nvarchar(10), @iCheckSerial) + N' order by serial asc'

	exec sp_executesql @nQuery
-- End_






GO
/****** Object:  StoredProcedure [dbo].[pSearch_InvenItem]    Script Date: 12/01/2026 16:57:20 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO





-- 인벤 검색 프로시져 : tbl_inven, tbl_base 조인
CREATE PROCEDURE [dbo].[pSearch_InvenItem]
@iTopCount int,
@iCheckSerial int
AS
-- Begin_
	declare @nQuery nvarchar(3020)
	Set @nQuery = N''
	Set @nQuery = @nQuery + N'Select top ' + convert(nvarchar(3), @iTopCount) + N' '
	Set @nQuery = @nQuery + N'b.serial, b.name, b.account, b.accountserial, b.lv, '
	Set @nQuery = @nQuery + N'I.K0,I.D0,I.U0, I.K1,I.D1,I.U1, I.K2,I.D2,I.U2, I.K3,I.D3,I.U3, I.K4,I.D4,I.U4, I.K5,I.D5,I.U5, I.K6,I.D6,I.U6, I.K7,I.D7,I.U7, I.K8,I.D8,I.U8, I.K9,I.D9,I.U9,  '
 	Set @nQuery = @nQuery + N'I.K10,I.D10,I.U10, I.K11,I.D11,I.U11, I.K12,I.D12,I.U12, I.K13,I.D13,I.U13, I.K14,I.D14,I.U14, I.K15,I.D15,I.U15, I.K16,I.D16,I.U16, I.K17,I.D17,I.U17, I.K18,I.D18,I.U18, I.K19,I.D19,I.U19,  '
 	Set @nQuery = @nQuery + N'I.K20,I.D20,I.U20, I.K21,I.D21,I.U21, I.K22,I.D22,I.U22, I.K23,I.D23,I.U23, I.K24,I.D24,I.U24, I.K25,I.D25,I.U25, I.K26,I.D26,I.U26, I.K27,I.D27,I.U27, I.K28,I.D28,I.U28, I.K29,I.D29,I.U29,  '
 	Set @nQuery = @nQuery + N'I.K30,I.D30,I.U30, I.K31,I.D31,I.U31, I.K32,I.D32,I.U32, I.K33,I.D33,I.U33, I.K34,I.D34,I.U34, I.K35,I.D35,I.U35, I.K36,I.D36,I.U36, I.K37,I.D37,I.U37, I.K38,I.D38,I.U38, I.K39,I.D39,I.U39,  '
	Set @nQuery = @nQuery + N'I.K40,I.D40,I.U40, I.K41,I.D41,I.U41, I.K42,I.D42,I.U42, I.K43,I.D43,I.U43, I.K44,I.D44,I.U44, I.K45,I.D45,I.U45, I.K46,I.D46,I.U46, I.K47,I.D47,I.U47, I.K48,I.D48,I.U48, I.K49,I.D49,I.U49,  '
	Set @nQuery = @nQuery + N'I.K50,I.D50,I.U50, I.K51,I.D51,I.U51, I.K52,I.D52,I.U52, I.K53,I.D53,I.U53, I.K54,I.D54,I.U54, I.K55,I.D55,I.U55, I.K56,I.D56,I.U56, I.K57,I.D57,I.U57, I.K58,I.D58,I.U58, I.K59,I.D59,I.U59,  '
	Set @nQuery = @nQuery + N'I.K60,I.D60,I.U60, I.K61,I.D61,I.U61, I.K62,I.D62,I.U62, I.K63,I.D63,I.U63, I.K64,I.D64,I.U64, I.K65,I.D65,I.U65, I.K66,I.D66,I.U66, I.K67,I.D67,I.U67, I.K68,I.D68,I.U68, I.K69,I.D69,I.U69,  '
	Set @nQuery = @nQuery + N'I.K70,I.D70,I.U70, I.K71,I.D71,I.U71, I.K72,I.D72,I.U72, I.K73,I.D73,I.U73, I.K74,I.D74,I.U74, I.K75,I.D75,I.U75, I.K76,I.D76,I.U76, I.K77,I.D77,I.U77, I.K78,I.D78,I.U78, I.K79,I.D79,I.U79,  '
	Set @nQuery = @nQuery + N'I.K80,I.D80,I.U80, I.K81,I.D81,I.U81, I.K82,I.D82,I.U82, I.K83,I.D83,I.U83, I.K84,I.D84,I.U84, I.K85,I.D85,I.U85, I.K86,I.D86,I.U86, I.K87,I.D87,I.U87, I.K88,I.D88,I.U88, I.K89,I.D89,I.U89,  '
	Set @nQuery = @nQuery + N'I.K90,I.D90,I.U90, I.K91,I.D91,I.U91, I.K92,I.D92,I.U92, I.K93,I.D93,I.U93, I.K94,I.D94,I.U94, I.K95,I.D95,I.U95, I.K96,I.D96,I.U96, I.K97,I.D97,I.U97, I.K98,I.D98,I.U98, I.K99,I.D99,I.U99  '
	Set @nQuery = @nQuery + N'From tbl_inven as i inner join tbl_base as b on i.serial = b.serial '
	Set @nQuery = @nQuery + N'Where b.dck = 0 and b.accountserial < 2000000000 and b.serial >  ' + convert(nvarchar(10), @iCheckSerial) + N' order by b.serial asc'
--	Set @nQuery = @nQuery + N'Where b.dck = 0 and b.serial >  ' + convert(nvarchar(10), @iCheckSerial) + N' order by b.serial asc'

	exec sp_executesql @nQuery
-- End_






GO
/****** Object:  StoredProcedure [dbo].[pSearch_SelectCount]    Script Date: 12/01/2026 16:57:20 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO



/*
DROP PROCEDURE pSearch_SelectCount
DROP PROCEDURE pSearch_AccItem
DROP PROCEDURE pSearch_EquipItem
DROP PROCEDURE pSearch_InvenItem
DROP PROCEDURE pSearch_TrunkItem
*/

-- Acc, Equip, Inven, Trunk의 대상 전체 갯수
CREATE PROCEDURE [dbo].[pSearch_SelectCount]
@byType tinyint
AS
-- Begin_
	declare @nQuery nvarchar(300)

	-- Acc : 착용장신구
	if(@byType = 1) 
	begin
		Set @nQuery = N'select count(*) From tbl_general as g inner join tbl_base as b on g.serial = b.serial Where b.dck = 0 and b.accountserial < 2000000000'
--		Set @nQuery = N'select count(*) From tbl_general as g inner join tbl_base as b on g.serial = b.serial Where b.dck = 0'
	end

	-- Equip : 착용장비
	else if(@byType = 2) 
	begin
		Set @nQuery = N'select count(*) From tbl_base Where dck = 0 and accountserial < 2000000000'
--		Set @nQuery = N'select count(*) From tbl_base Where dck = 0'
	end

	-- Inven : 인벤
	else if(@byType = 3) 
	begin
		Set @nQuery = N'select count(*) From tbl_inven as i inner join tbl_base as b on i.serial = b.serial Where b.dck = 0 and b.accountserial < 2000000000'
--		Set @nQuery = N'select count(*) From tbl_inven as i inner join tbl_base as b on i.serial = b.serial Where b.dck = 0'
	end

	-- Trunk : 창고
	else if(@byType = 4) 
	begin
		Set @nQuery = N'select count(*) from (select count(*) as ret From tbl_accounttrunk as a inner join tbl_base as b on a.accountserial = b.accountserial  Where b.dck = 0 and b.accountserial < 2000000000 Group by b.accountserial, b.account) as Total'
--		Set @nQuery = N'select count(*) from (select count(*) as ret From tbl_accounttrunk as a inner join tbl_base as b on a.accountserial = b.accountserial  Where b.dck = 0 Group by b.accountserial, b.account) as Total'
	end

	else
	begin
		Set @nQuery = N'select 0'
	end

	exec sp_executesql @nQuery
-- End_






GO
/****** Object:  StoredProcedure [dbo].[pSearch_TrunkItem]    Script Date: 12/01/2026 16:57:20 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO





-- 창고 검색 프로시져
CREATE PROCEDURE [dbo].[pSearch_TrunkItem]
@iTopCount int,
@iCheckSerial int
AS
-- Begin_
	declare @nQuery nvarchar(3000)
	Set @nQuery = N''
	Set @nQuery = @nQuery + N'Select AI.accountserial, AI.account, '
	Set @nQuery = @nQuery + N'T.K0,T.D0,T.U0,T.K1,T.D1,T.U1,T.K2,T.D2,T.U2,T.K3,T.D3,T.U3,T.K4,T.D4,T.U4,'
	Set @nQuery = @nQuery + N'T.K5,T.D5,T.U5,T.K6,T.D6,T.U6,T.K7,T.D7,T.U7,T.K8,T.D8,T.U8,T.K9,T.D9,T.U9,'

	Set @nQuery = @nQuery + N'T.K10,T.D10,T.U10,T.K11,T.D11,T.U11,T.K12,T.D12,T.U12,T.K13,T.D13,T.U13,T.K14,T.D14,T.U14,'
	Set @nQuery = @nQuery + N'T.K15,T.D15,T.U15,T.K16,T.D16,T.U16,T.K17,T.D17,T.U17,T.K18,T.D18,T.U18,T.K19,T.D19,T.U19,'

	Set @nQuery = @nQuery + N'T.K20,T.D20,T.U20,T.K21,T.D21,T.U21,T.K22,T.D22,T.U22,T.K23,T.D23,T.U23,T.K24,T.D24,T.U24,'
	Set @nQuery = @nQuery + N'T.K25,T.D25,T.U25,T.K26,T.D26,T.U26,T.K27,T.D27,T.U27,T.K28,T.D28,T.U28,T.K29,T.D29,T.U29,'

	Set @nQuery = @nQuery + N'T.K30,T.D30,T.U30,T.K31,T.D31,T.U31,T.K32,T.D32,T.U32,T.K33,T.D33,T.U33,T.K34,T.D34,T.U34,'
	Set @nQuery = @nQuery + N'T.K35,T.D35,T.U35,T.K36,T.D36,T.U36,T.K37,T.D37,T.U37,T.K38,T.D38,T.U38,T.K39,T.D39,T.U39,'

	Set @nQuery = @nQuery + N'T.K40,T.D40,T.U40,T.K41,T.D41,T.U41,T.K42,T.D42,T.U42,T.K43,T.D43,T.U43,T.K44,T.D44,T.U44,'
	Set @nQuery = @nQuery + N'T.K45,T.D45,T.U45,T.K46,T.D46,T.U46,T.K47,T.D47,T.U47,T.K48,T.D48,T.U48,T.K49,T.D49,T.U49,'

	Set @nQuery = @nQuery + N'T.K50,T.D50,T.U50,T.K51,T.D51,T.U51,T.K52,T.D52,T.U52,T.K53,T.D53,T.U53,T.K54,T.D54,T.U54,'
	Set @nQuery = @nQuery + N'T.K55,T.D55,T.U55,T.K56,T.D56,T.U56,T.K57,T.D57,T.U57,T.K58,T.D58,T.U58,T.K59,T.D59,T.U59,'

	Set @nQuery = @nQuery + N'T.K60,T.D60,T.U60,T.K61,T.D61,T.U61,T.K62,T.D62,T.U62,T.K63,T.D63,T.U63,T.K64,T.D64,T.U64,'
	Set @nQuery = @nQuery + N'T.K65,T.D65,T.U65,T.K66,T.D66,T.U66,T.K67,T.D67,T.U67,T.K68,T.D68,T.U68,T.K69,T.D69,T.U69,'

	Set @nQuery = @nQuery + N'T.K70,T.D70,T.U70,T.K71,T.D71,T.U71,T.K72,T.D72,T.U72,T.K73,T.D73,T.U73,T.K74,T.D74,T.U74,'
	Set @nQuery = @nQuery + N'T.K75,T.D75,T.U75,T.K76,T.D76,T.U76,T.K77,T.D77,T.U77,T.K78,T.D78,T.U78,T.K79,T.D79,T.U79,'

	Set @nQuery = @nQuery + N'T.K80,T.D80,T.U80,T.K81,T.D81,T.U81,T.K82,T.D82,T.U82,T.K83,T.D83,T.U83,T.K84,T.D84,T.U84,'
	Set @nQuery = @nQuery + N'T.K85,T.D85,T.U85,T.K86,T.D86,T.U86,T.K87,T.D87,T.U87,T.K88,T.D88,T.U88,T.K89,T.D89,T.U89,'

	Set @nQuery = @nQuery + N'T.K90,T.D90,T.U90,T.K91,T.D91,T.U91,T.K92,T.D92,T.U92,T.K93,T.D93,T.U93,T.K94,T.D94,T.U94,'
	Set @nQuery = @nQuery + N'T.K95,T.D95,T.U95,T.K96,T.D96,T.U96,T.K97,T.D97,T.U97,T.K98,T.D98,T.U98,T.K99,T.D99,T.U99 '
	Set @nQuery = @nQuery + N'From tbl_accounttrunk as T inner join  '
		Set @nQuery = @nQuery + N'( '
			Set @nQuery = @nQuery + N'Select top  ' + convert(nvarchar(3), @iTopCount) + N' '
			Set @nQuery = @nQuery + N'b.accountserial, b.account '
			Set @nQuery = @nQuery + N'From tbl_accounttrunk as a inner join tbl_base as b on a.accountserial = b.accountserial  '
			Set @nQuery = @nQuery + N'Where b.dck = 0 and b.accountserial < 2000000000 and a.accountserial > ' + convert(nvarchar(10), @iCheckSerial) + N' '
--			Set @nQuery = @nQuery + N'Where b.dck = 0 and a.accountserial > ' + convert(nvarchar(10), @iCheckSerial) + N' '
			Set @nQuery = @nQuery + N'Group by b.accountserial, b.account '
			Set @nQuery = @nQuery + N'Order by b.accountserial asc '
		Set @nQuery = @nQuery + N') as AI on T.accountserial = AI.accountserial '
	Set @nQuery = @nQuery + N'Order by Ai.accountserial '

	exec sp_executesql @nQuery
-- End_






GO
/****** Object:  StoredProcedure [dbo].[pSelect_Account_Vote_Cnt]    Script Date: 12/01/2026 16:57:20 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER OFF
GO






/****** 개체: 저장 프로시저 하나의 계정에 대한 누적 투표 수를 조회 한다. ******/ 
CREATE  PROCEDURE [dbo].[pSelect_Account_Vote_Cnt]
@accserial 		 int,
@accum			 int	output

As
SET NOCOUNT ON

Set 	@accum = 0

Select @accum = s.IsVoted + @accum
From [dbo].[tbl_Supplement] as s join [dbo].[tbl_base] as b on s.serial = b.serial
Where b.accountserial = @accserial AND b.DCK = 0


IF (@@ERROR <> 0 )BEGIN	
	RETURN @@ERROR
END

SET NOCOUNT OFF





GO
/****** Object:  StoredProcedure [dbo].[pSelect_AccountByAvatorName]    Script Date: 12/01/2026 16:57:20 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO








CREATE PROCEDURE [dbo].[pSelect_AccountByAvatorName]
@szName varchar(17)
AS
select account from [dbo].[tbl_base] where name=@szName and DCK=0







GO
/****** Object:  StoredProcedure [dbo].[pSelect_AccountTrunk_0]    Script Date: 12/01/2026 16:57:20 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO








CREATE PROCEDURE [dbo].[pSelect_AccountTrunk_0]
@serial int
AS
SELECT 
cast(TrunkPass as varchar(13) ), Dalant0, Gold0, 
HintIndex, HintAnswer,EstSlot,
K0,D0,U0,R0,
K1,D1,U1,R1,
K2,D2,U2,R2,
K3,D3,U3,R3,
K4,D4,U4,R4,
K5,D5,U5,R5,
K6,D6,U6,R6,
K7,D7,U7,R7,
K8,D8,U8,R8,
K9,D9,U9,R9,
K10,D10,U10,R10,
K11,D11,U11,R11,
K12,D12,U12,R12,
K13,D13,U13,R13,
K14,D14,U14,R14,
K15,D15,U15,R15,
K16,D16,U16,R16,
K17,D17,U17,R17,
K18,D18,U18,R18,
K19,D19,U19,R19,

K20,D20,U20,R20,
K21,D21,U21,R21,
K22,D22,U22,R22,
K23,D23,U23,R23,
K24,D24,U24,R24,
K25,D25,U25,R25,
K26,D26,U26,R26,
K27,D27,U27,R27,
K28,D28,U28,R28,
K29,D29,U29,R29,

K30,D30,U30,R30,
K31,D31,U31,R31,
K32,D32,U32,R32,
K33,D33,U33,R33,
K34,D34,U34,R34,
K35,D35,U35,R35,
K36,D36,U36,R36,
K37,D37,U37,R37,
K38,D38,U38,R38,
K39,D39,U39,R39,

K40,D40,U40,R40,
K41,D41,U41,R41,
K42,D42,U42,R42,
K43,D43,U43,R43,
K44,D44,U44,R44,
K45,D45,U45,R45,
K46,D46,U46,R46,
K47,D47,U47,R47,
K48,D48,U48,R48,
K49,D49,U49,R49,

K50,D50,U50,R50,
K51,D51,U51,R51,
K52,D52,U52,R52,
K53,D53,U53,R53,
K54,D54,U54,R54,
K55,D55,U55,R55,
K56,D56,U56,R56,
K57,D57,U57,R57,
K58,D58,U58,R58,
K59,D59,U59,R59,

K60,D60,U60,R60,
K61,D61,U61,R61,
K62,D62,U62,R62,
K63,D63,U63,R63,
K64,D64,U64,R64,
K65,D65,U65,R65,
K66,D66,U66,R66,
K67,D67,U67,R67,
K68,D68,U68,R68,
K69,D69,U69,R69,

K70,D70,U70,R70,
K71,D71,U71,R71,
K72,D72,U72,R72,
K73,D73,U73,R73,
K74,D74,U74,R74,
K75,D75,U75,R75,
K76,D76,U76,R76,
K77,D77,U77,R77,
K78,D78,U78,R78,
K79,D79,U79,R79,

K80,D80,U80,R80,
K81,D81,U81,R81,
K82,D82,U82,R82,
K83,D83,U83,R83,
K84,D84,U84,R84,
K85,D85,U85,R85,
K86,D86,U86,R86,
K87,D87,U87,R87,
K88,D88,U88,R88,
K89,D89,U89,R89,

K90,D90,U90,R90,
K91,D91,U91,R91,
K92,D92,U92,R92,
K93,D93,U93,R93,
K94,D94,U94,R94,
K95,D95,U95,R95,
K96,D96,U96,R96,
K97,D97,U97,R97,
K98,D98,U98,R98,
K99,D99,U99,R99

FROM [dbo].[tbl_AccountTrunk]
WHERE AccountSerial = @serial







GO
/****** Object:  StoredProcedure [dbo].[pSelect_AccountTrunk_1]    Script Date: 12/01/2026 16:57:20 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO








CREATE PROCEDURE [dbo].[pSelect_AccountTrunk_1]
@serial int
AS
SELECT 
cast(TrunkPass as varchar(13) ), Dalant1, Gold1, 
HintIndex, HintAnswer,EstSlot,
K0,D0,U0,R0,
K1,D1,U1,R1,
K2,D2,U2,R2,
K3,D3,U3,R3,
K4,D4,U4,R4,
K5,D5,U5,R5,
K6,D6,U6,R6,
K7,D7,U7,R7,
K8,D8,U8,R8,
K9,D9,U9,R9,
K10,D10,U10,R10,
K11,D11,U11,R11,
K12,D12,U12,R12,
K13,D13,U13,R13,
K14,D14,U14,R14,
K15,D15,U15,R15,
K16,D16,U16,R16,
K17,D17,U17,R17,
K18,D18,U18,R18,
K19,D19,U19,R19,

K20,D20,U20,R20,
K21,D21,U21,R21,
K22,D22,U22,R22,
K23,D23,U23,R23,
K24,D24,U24,R24,
K25,D25,U25,R25,
K26,D26,U26,R26,
K27,D27,U27,R27,
K28,D28,U28,R28,
K29,D29,U29,R29,

K30,D30,U30,R30,
K31,D31,U31,R31,
K32,D32,U32,R32,
K33,D33,U33,R33,
K34,D34,U34,R34,
K35,D35,U35,R35,
K36,D36,U36,R36,
K37,D37,U37,R37,
K38,D38,U38,R38,
K39,D39,U39,R39,

K40,D40,U40,R40,
K41,D41,U41,R41,
K42,D42,U42,R42,
K43,D43,U43,R43,
K44,D44,U44,R44,
K45,D45,U45,R45,
K46,D46,U46,R46,
K47,D47,U47,R47,
K48,D48,U48,R48,
K49,D49,U49,R49,

K50,D50,U50,R50,
K51,D51,U51,R51,
K52,D52,U52,R52,
K53,D53,U53,R53,
K54,D54,U54,R54,
K55,D55,U55,R55,
K56,D56,U56,R56,
K57,D57,U57,R57,
K58,D58,U58,R58,
K59,D59,U59,R59,

K60,D60,U60,R60,
K61,D61,U61,R61,
K62,D62,U62,R62,
K63,D63,U63,R63,
K64,D64,U64,R64,
K65,D65,U65,R65,
K66,D66,U66,R66,
K67,D67,U67,R67,
K68,D68,U68,R68,
K69,D69,U69,R69,

K70,D70,U70,R70,
K71,D71,U71,R71,
K72,D72,U72,R72,
K73,D73,U73,R73,
K74,D74,U74,R74,
K75,D75,U75,R75,
K76,D76,U76,R76,
K77,D77,U77,R77,
K78,D78,U78,R78,
K79,D79,U79,R79,

K80,D80,U80,R80,
K81,D81,U81,R81,
K82,D82,U82,R82,
K83,D83,U83,R83,
K84,D84,U84,R84,
K85,D85,U85,R85,
K86,D86,U86,R86,
K87,D87,U87,R87,
K88,D88,U88,R88,
K89,D89,U89,R89,

K90,D90,U90,R90,
K91,D91,U91,R91,
K92,D92,U92,R92,
K93,D93,U93,R93,
K94,D94,U94,R94,
K95,D95,U95,R95,
K96,D96,U96,R96,
K97,D97,U97,R97,
K98,D98,U98,R98,
K99,D99,U99,R99

FROM [dbo].[tbl_AccountTrunk]
WHERE AccountSerial = @serial







GO
/****** Object:  StoredProcedure [dbo].[pSelect_AccountTrunk_2]    Script Date: 12/01/2026 16:57:20 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO








CREATE PROCEDURE [dbo].[pSelect_AccountTrunk_2]
@serial int
AS
SELECT 
cast(TrunkPass as varchar(13) ), Dalant2, Gold2,
HintIndex, HintAnswer,EstSlot,
K0,D0,U0,R0,
K1,D1,U1,R1,
K2,D2,U2,R2,
K3,D3,U3,R3,
K4,D4,U4,R4,
K5,D5,U5,R5,
K6,D6,U6,R6,
K7,D7,U7,R7,
K8,D8,U8,R8,
K9,D9,U9,R9,
K10,D10,U10,R10,
K11,D11,U11,R11,
K12,D12,U12,R12,
K13,D13,U13,R13,
K14,D14,U14,R14,
K15,D15,U15,R15,
K16,D16,U16,R16,
K17,D17,U17,R17,
K18,D18,U18,R18,
K19,D19,U19,R19,

K20,D20,U20,R20,
K21,D21,U21,R21,
K22,D22,U22,R22,
K23,D23,U23,R23,
K24,D24,U24,R24,
K25,D25,U25,R25,
K26,D26,U26,R26,
K27,D27,U27,R27,
K28,D28,U28,R28,
K29,D29,U29,R29,

K30,D30,U30,R30,
K31,D31,U31,R31,
K32,D32,U32,R32,
K33,D33,U33,R33,
K34,D34,U34,R34,
K35,D35,U35,R35,
K36,D36,U36,R36,
K37,D37,U37,R37,
K38,D38,U38,R38,
K39,D39,U39,R39,

K40,D40,U40,R40,
K41,D41,U41,R41,
K42,D42,U42,R42,
K43,D43,U43,R43,
K44,D44,U44,R44,
K45,D45,U45,R45,
K46,D46,U46,R46,
K47,D47,U47,R47,
K48,D48,U48,R48,
K49,D49,U49,R49,

K50,D50,U50,R50,
K51,D51,U51,R51,
K52,D52,U52,R52,
K53,D53,U53,R53,
K54,D54,U54,R54,
K55,D55,U55,R55,
K56,D56,U56,R56,
K57,D57,U57,R57,
K58,D58,U58,R58,
K59,D59,U59,R59,

K60,D60,U60,R60,
K61,D61,U61,R61,
K62,D62,U62,R62,
K63,D63,U63,R63,
K64,D64,U64,R64,
K65,D65,U65,R65,
K66,D66,U66,R66,
K67,D67,U67,R67,
K68,D68,U68,R68,
K69,D69,U69,R69,

K70,D70,U70,R70,
K71,D71,U71,R71,
K72,D72,U72,R72,
K73,D73,U73,R73,
K74,D74,U74,R74,
K75,D75,U75,R75,
K76,D76,U76,R76,
K77,D77,U77,R77,
K78,D78,U78,R78,
K79,D79,U79,R79,

K80,D80,U80,R80,
K81,D81,U81,R81,
K82,D82,U82,R82,
K83,D83,U83,R83,
K84,D84,U84,R84,
K85,D85,U85,R85,
K86,D86,U86,R86,
K87,D87,U87,R87,
K88,D88,U88,R88,
K89,D89,U89,R89,

K90,D90,U90,R90,
K91,D91,U91,R91,
K92,D92,U92,R92,
K93,D93,U93,R93,
K94,D94,U94,R94,
K95,D95,U95,R95,
K96,D96,U96,R96,
K97,D97,U97,R97,
K98,D98,U98,R98,
K99,D99,U99,R99

FROM [dbo].[tbl_AccountTrunk]
WHERE AccountSerial = @serial







GO
/****** Object:  StoredProcedure [dbo].[pSelect_AccountTrunk_20061115_0]    Script Date: 12/01/2026 16:57:20 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO



/** 벨라토 종족 창고 읽어 오기 */
CREATE PROCEDURE [dbo].[pSelect_AccountTrunk_20061115_0]
@serial int
AS
SELECT 
cast(TrunkPass as varchar(13) ), Dalant0, Gold0, 
HintIndex, HintAnswer,EstSlot,

K0,D0,U0,R0,S0,T0,
K1,D1,U1,R1,S1,T1,
K2,D2,U2,R2,S2,T2,
K3,D3,U3,R3,S3,T3,
K4,D4,U4,R4,S4,T4,
K5,D5,U5,R5,S5,T5,
K6,D6,U6,R6,S6,T6,
K7,D7,U7,R7,S7,T7,
K8,D8,U8,R8,S8,T8,
K9,D9,U9,R9,S9,T9,
K10,D10,U10,R10,S10,T10,
K11,D11,U11,R11,S11,T11,
K12,D12,U12,R12,S12,T12,
K13,D13,U13,R13,S13,T13,
K14,D14,U14,R14,S14,T14,
K15,D15,U15,R15,S15,T15,
K16,D16,U16,R16,S16,T16,
K17,D17,U17,R17,S17,T17,
K18,D18,U18,R18,S18,T18,
K19,D19,U19,R19,S19,T19,

K20,D20,U20,R20,S20,T20,
K21,D21,U21,R21,S21,T21,
K22,D22,U22,R22,S22,T22,
K23,D23,U23,R23,S23,T23,
K24,D24,U24,R24,S24,T24,
K25,D25,U25,R25,S25,T25,
K26,D26,U26,R26,S26,T26,
K27,D27,U27,R27,S27,T27,
K28,D28,U28,R28,S28,T28,
K29,D29,U29,R29,S29,T29,

K30,D30,U30,R30,S30,T30,
K31,D31,U31,R31,S31,T31,
K32,D32,U32,R32,S32,T32,
K33,D33,U33,R33,S33,T33,
K34,D34,U34,R34,S34,T34,
K35,D35,U35,R35,S35,T35,
K36,D36,U36,R36,S36,T36,
K37,D37,U37,R37,S37,T37,
K38,D38,U38,R38,S38,T38,
K39,D39,U39,R39,S39,T39,

K40,D40,U40,R40,S40,T40,
K41,D41,U41,R41,S41,T41,
K42,D42,U42,R42,S42,T42,
K43,D43,U43,R43,S43,T43,
K44,D44,U44,R44,S44,T44,
K45,D45,U45,R45,S45,T45,
K46,D46,U46,R46,S46,T46,
K47,D47,U47,R47,S47,T47,
K48,D48,U48,R48,S48,T48,
K49,D49,U49,R49,S49,T49,

K50,D50,U50,R50,S50,T50,
K51,D51,U51,R51,S51,T51,
K52,D52,U52,R52,S52,T52,
K53,D53,U53,R53,S53,T53,
K54,D54,U54,R54,S54,T54,
K55,D55,U55,R55,S55,T55,
K56,D56,U56,R56,S56,T56,
K57,D57,U57,R57,S57,T57,
K58,D58,U58,R58,S58,T58,
K59,D59,U59,R59,S59,T59,

K60,D60,U60,R60,S60,T60,
K61,D61,U61,R61,S61,T61,
K62,D62,U62,R62,S62,T62,
K63,D63,U63,R63,S63,T63,
K64,D64,U64,R64,S64,T64,
K65,D65,U65,R65,S65,T65,
K66,D66,U66,R66,S66,T66,
K67,D67,U67,R67,S67,T67,
K68,D68,U68,R68,S68,T68,
K69,D69,U69,R69,S69,T69,

K70,D70,U70,R70,S70,T70,
K71,D71,U71,R71,S71,T71,
K72,D72,U72,R72,S72,T72,
K73,D73,U73,R73,S73,T73,
K74,D74,U74,R74,S74,T74,
K75,D75,U75,R75,S75,T75,
K76,D76,U76,R76,S76,T76,
K77,D77,U77,R77,S77,T77,
K78,D78,U78,R78,S78,T78,
K79,D79,U79,R79,S79,T79,

K80,D80,U80,R80,S80,T80,
K81,D81,U81,R81,S81,T81,
K82,D82,U82,R82,S82,T82,
K83,D83,U83,R83,S83,T83,
K84,D84,U84,R84,S84,T84,
K85,D85,U85,R85,S85,T85,
K86,D86,U86,R86,S86,T86,
K87,D87,U87,R87,S87,T87,
K88,D88,U88,R88,S88,T88,
K89,D89,U89,R89,S89,T89,

K90,D90,U90,R90,S90,T90,
K91,D91,U91,R91,S91,T91,
K92,D92,U92,R92,S92,T92,
K93,D93,U93,R93,S93,T93,
K94,D94,U94,R94,S94,T94,
K95,D95,U95,R95,S95,T95,
K96,D96,U96,R96,S96,T96,
K97,D97,U97,R97,S97,T97,
K98,D98,U98,R98,S98,T98,
K99,D99,U99,R99,S99,T99

FROM [dbo].[tbl_AccountTrunk]
WHERE AccountSerial = @serial






GO
/****** Object:  StoredProcedure [dbo].[pSelect_AccountTrunk_20061115_1]    Script Date: 12/01/2026 16:57:20 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO








/** 코라종족 창고 읽어 오기 */
CREATE PROCEDURE [dbo].[pSelect_AccountTrunk_20061115_1]
@serial int
AS
SELECT 
cast(TrunkPass as varchar(13) ), Dalant1, Gold1, 
HintIndex, HintAnswer,EstSlot,
K0,D0,U0,R0,S0,T0,
K1,D1,U1,R1,S1,T1,
K2,D2,U2,R2,S2,T2,
K3,D3,U3,R3,S3,T3,
K4,D4,U4,R4,S4,T4,
K5,D5,U5,R5,S5,T5,
K6,D6,U6,R6,S6,T6,
K7,D7,U7,R7,S7,T7,
K8,D8,U8,R8,S8,T8,
K9,D9,U9,R9,S9,T9,
K10,D10,U10,R10,S10,T10,
K11,D11,U11,R11,S11,T11,
K12,D12,U12,R12,S12,T12,
K13,D13,U13,R13,S13,T13,
K14,D14,U14,R14,S14,T14,
K15,D15,U15,R15,S15,T15,
K16,D16,U16,R16,S16,T16,
K17,D17,U17,R17,S17,T17,
K18,D18,U18,R18,S18,T18,
K19,D19,U19,R19,S19,T19,

K20,D20,U20,R20,S20,T20,
K21,D21,U21,R21,S21,T21,
K22,D22,U22,R22,S22,T22,
K23,D23,U23,R23,S23,T23,
K24,D24,U24,R24,S24,T24,
K25,D25,U25,R25,S25,T25,
K26,D26,U26,R26,S26,T26,
K27,D27,U27,R27,S27,T27,
K28,D28,U28,R28,S28,T28,
K29,D29,U29,R29,S29,T29,

K30,D30,U30,R30,S30,T30,
K31,D31,U31,R31,S31,T31,
K32,D32,U32,R32,S32,T32,
K33,D33,U33,R33,S33,T33,
K34,D34,U34,R34,S34,T34,
K35,D35,U35,R35,S35,T35,
K36,D36,U36,R36,S36,T36,
K37,D37,U37,R37,S37,T37,
K38,D38,U38,R38,S38,T38,
K39,D39,U39,R39,S39,T39,

K40,D40,U40,R40,S40,T40,
K41,D41,U41,R41,S41,T41,
K42,D42,U42,R42,S42,T42,
K43,D43,U43,R43,S43,T43,
K44,D44,U44,R44,S44,T44,
K45,D45,U45,R45,S45,T45,
K46,D46,U46,R46,S46,T46,
K47,D47,U47,R47,S47,T47,
K48,D48,U48,R48,S48,T48,
K49,D49,U49,R49,S49,T49,

K50,D50,U50,R50,S50,T50,
K51,D51,U51,R51,S51,T51,
K52,D52,U52,R52,S52,T52,
K53,D53,U53,R53,S53,T53,
K54,D54,U54,R54,S54,T54,
K55,D55,U55,R55,S55,T55,
K56,D56,U56,R56,S56,T56,
K57,D57,U57,R57,S57,T57,
K58,D58,U58,R58,S58,T58,
K59,D59,U59,R59,S59,T59,

K60,D60,U60,R60,S60,T60,
K61,D61,U61,R61,S61,T61,
K62,D62,U62,R62,S62,T62,
K63,D63,U63,R63,S63,T63,
K64,D64,U64,R64,S64,T64,
K65,D65,U65,R65,S65,T65,
K66,D66,U66,R66,S66,T66,
K67,D67,U67,R67,S67,T67,
K68,D68,U68,R68,S68,T68,
K69,D69,U69,R69,S69,T69,

K70,D70,U70,R70,S70,T70,
K71,D71,U71,R71,S71,T71,
K72,D72,U72,R72,S72,T72,
K73,D73,U73,R73,S73,T73,
K74,D74,U74,R74,S74,T74,
K75,D75,U75,R75,S75,T75,
K76,D76,U76,R76,S76,T76,
K77,D77,U77,R77,S77,T77,
K78,D78,U78,R78,S78,T78,
K79,D79,U79,R79,S79,T79,

K80,D80,U80,R80,S80,T80,
K81,D81,U81,R81,S81,T81,
K82,D82,U82,R82,S82,T82,
K83,D83,U83,R83,S83,T83,
K84,D84,U84,R84,S84,T84,
K85,D85,U85,R85,S85,T85,
K86,D86,U86,R86,S86,T86,
K87,D87,U87,R87,S87,T87,
K88,D88,U88,R88,S88,T88,
K89,D89,U89,R89,S89,T89,

K90,D90,U90,R90,S90,T90,
K91,D91,U91,R91,S91,T91,
K92,D92,U92,R92,S92,T92,
K93,D93,U93,R93,S93,T93,
K94,D94,U94,R94,S94,T94,
K95,D95,U95,R95,S95,T95,
K96,D96,U96,R96,S96,T96,
K97,D97,U97,R97,S97,T97,
K98,D98,U98,R98,S98,T98,
K99,D99,U99,R99,S99,T99

FROM [dbo].[tbl_AccountTrunk]
WHERE AccountSerial = @serial






GO
/****** Object:  StoredProcedure [dbo].[pSelect_AccountTrunk_20061115_2]    Script Date: 12/01/2026 16:57:20 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO








/* 아크레시아 창고 읽어 오기 */
CREATE PROCEDURE [dbo].[pSelect_AccountTrunk_20061115_2]
@serial int
AS
SELECT 
cast(TrunkPass as varchar(13) ), Dalant2, Gold2,
HintIndex, HintAnswer,EstSlot,
K0,D0,U0,R0,S0,T0,
K1,D1,U1,R1,S1,T1,
K2,D2,U2,R2,S2,T2,
K3,D3,U3,R3,S3,T3,
K4,D4,U4,R4,S4,T4,
K5,D5,U5,R5,S5,T5,
K6,D6,U6,R6,S6,T6,
K7,D7,U7,R7,S7,T7,
K8,D8,U8,R8,S8,T8,
K9,D9,U9,R9,S9,T9,
K10,D10,U10,R10,S10,T10,
K11,D11,U11,R11,S11,T11,
K12,D12,U12,R12,S12,T12,
K13,D13,U13,R13,S13,T13,
K14,D14,U14,R14,S14,T14,
K15,D15,U15,R15,S15,T15,
K16,D16,U16,R16,S16,T16,
K17,D17,U17,R17,S17,T17,
K18,D18,U18,R18,S18,T18,
K19,D19,U19,R19,S19,T19,

K20,D20,U20,R20,S20,T20,
K21,D21,U21,R21,S21,T21,
K22,D22,U22,R22,S22,T22,
K23,D23,U23,R23,S23,T23,
K24,D24,U24,R24,S24,T24,
K25,D25,U25,R25,S25,T25,
K26,D26,U26,R26,S26,T26,
K27,D27,U27,R27,S27,T27,
K28,D28,U28,R28,S28,T28,
K29,D29,U29,R29,S29,T29,

K30,D30,U30,R30,S30,T30,
K31,D31,U31,R31,S31,T31,
K32,D32,U32,R32,S32,T32,
K33,D33,U33,R33,S33,T33,
K34,D34,U34,R34,S34,T34,
K35,D35,U35,R35,S35,T35,
K36,D36,U36,R36,S36,T36,
K37,D37,U37,R37,S37,T37,
K38,D38,U38,R38,S38,T38,
K39,D39,U39,R39,S39,T39,

K40,D40,U40,R40,S40,T40,
K41,D41,U41,R41,S41,T41,
K42,D42,U42,R42,S42,T42,
K43,D43,U43,R43,S43,T43,
K44,D44,U44,R44,S44,T44,
K45,D45,U45,R45,S45,T45,
K46,D46,U46,R46,S46,T46,
K47,D47,U47,R47,S47,T47,
K48,D48,U48,R48,S48,T48,
K49,D49,U49,R49,S49,T49,

K50,D50,U50,R50,S50,T50,
K51,D51,U51,R51,S51,T51,
K52,D52,U52,R52,S52,T52,
K53,D53,U53,R53,S53,T53,
K54,D54,U54,R54,S54,T54,
K55,D55,U55,R55,S55,T55,
K56,D56,U56,R56,S56,T56,
K57,D57,U57,R57,S57,T57,
K58,D58,U58,R58,S58,T58,
K59,D59,U59,R59,S59,T59,

K60,D60,U60,R60,S60,T60,
K61,D61,U61,R61,S61,T61,
K62,D62,U62,R62,S62,T62,
K63,D63,U63,R63,S63,T63,
K64,D64,U64,R64,S64,T64,
K65,D65,U65,R65,S65,T65,
K66,D66,U66,R66,S66,T66,
K67,D67,U67,R67,S67,T67,
K68,D68,U68,R68,S68,T68,
K69,D69,U69,R69,S69,T69,

K70,D70,U70,R70,S70,T70,
K71,D71,U71,R71,S71,T71,
K72,D72,U72,R72,S72,T72,
K73,D73,U73,R73,S73,T73,
K74,D74,U74,R74,S74,T74,
K75,D75,U75,R75,S75,T75,
K76,D76,U76,R76,S76,T76,
K77,D77,U77,R77,S77,T77,
K78,D78,U78,R78,S78,T78,
K79,D79,U79,R79,S79,T79,

K80,D80,U80,R80,S80,T80,
K81,D81,U81,R81,S81,T81,
K82,D82,U82,R82,S82,T82,
K83,D83,U83,R83,S83,T83,
K84,D84,U84,R84,S84,T84,
K85,D85,U85,R85,S85,T85,
K86,D86,U86,R86,S86,T86,
K87,D87,U87,R87,S87,T87,
K88,D88,U88,R88,S88,T88,
K89,D89,U89,R89,S89,T89,

K90,D90,U90,R90,S90,T90,
K91,D91,U91,R91,S91,T91,
K92,D92,U92,R92,S92,T92,
K93,D93,U93,R93,S93,T93,
K94,D94,U94,R94,S94,T94,
K95,D95,U95,R95,S95,T95,
K96,D96,U96,R96,S96,T96,
K97,D97,U97,R97,S97,T97,
K98,D98,U98,R98,S98,T98,
K99,D99,U99,R99,S99,T99

FROM [dbo].[tbl_AccountTrunk]
WHERE AccountSerial = @serial






GO
/****** Object:  StoredProcedure [dbo].[pSelect_AccountTrunk_Extend]    Script Date: 12/01/2026 16:57:20 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO



/** 확장된 창고 정보 읽어오기 */
CREATE  PROCEDURE [dbo].[pSelect_AccountTrunk_Extend]
@serial int
AS
SELECT 

EstSlot,
K0,D0,U0,R0,S0,T0,
K1,D1,U1,R1,S1,T1,
K2,D2,U2,R2,S2,T2,
K3,D3,U3,R3,S3,T3,
K4,D4,U4,R4,S4,T4,
K5,D5,U5,R5,S5,T5,
K6,D6,U6,R6,S6,T6,
K7,D7,U7,R7,S7,T7,
K8,D8,U8,R8,S8,T8,
K9,D9,U9,R9,S9,T9,

K10,D10,U10,R10,S10,T10,
K11,D11,U11,R11,S11,T11,
K12,D12,U12,R12,S12,T12,
K13,D13,U13,R13,S13,T13,
K14,D14,U14,R14,S14,T14,
K15,D15,U15,R15,S15,T15,
K16,D16,U16,R16,S16,T16,
K17,D17,U17,R17,S17,T17,
K18,D18,U18,R18,S18,T18,
K19,D19,U19,R19,S19,T19,

K20,D20,U20,R20,S20,T20,
K21,D21,U21,R21,S21,T21,
K22,D22,U22,R22,S22,T22,
K23,D23,U23,R23,S23,T23,
K24,D24,U24,R24,S24,T24,
K25,D25,U25,R25,S25,T25,
K26,D26,U26,R26,S26,T26,
K27,D27,U27,R27,S27,T27,
K28,D28,U28,R28,S28,T28,
K29,D29,U29,R29,S29,T29,

K30,D30,U30,R30,S30,T30,
K31,D31,U31,R31,S31,T31,
K32,D32,U32,R32,S32,T32,
K33,D33,U33,R33,S33,T33,
K34,D34,U34,R34,S34,T34,
K35,D35,U35,R35,S35,T35,
K36,D36,U36,R36,S36,T36,
K37,D37,U37,R37,S37,T37,
K38,D38,U38,R38,S38,T38,
K39,D39,U39,R39,S39,T39


FROM [dbo].[tbl_AccountTrunk_Extend]
WHERE AccountSerial = @serial







GO
/****** Object:  StoredProcedure [dbo].[pselect_aminepersonal_inven]    Script Date: 12/01/2026 16:57:20 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO







create procedure [dbo].[pselect_aminepersonal_inven]
@nAvatorSerial int
as
select top 1 K0,N0,K1,N1,K2,N2,K3,N3,K4,N4,K5,N5,K6,N6,K7,N7,K8,N8,K9,N9,
	     K10,N10,K11,N11,K12,N12,K13,N13,K14,N14,K15,N15,K16,N16,K17,N17,K18,N18,K19,N19,
  	     K20,N20,K21,N21,K22,N22,K23,N23,K24,N24,K25,N25,K26,N26,K27,N27,K28,N28,K29,N29,
	     K30,N30,K31,N31,K32,N32,K33,N33,K34,N34,K35,N35,K36,N36,K37,N37,K38,N38,K39,N39
from [dbo].[tbl_aminepersonal_inven]
where dck = 0 and avatorserial = @navatorserial







GO
/****** Object:  StoredProcedure [dbo].[pSelect_AnimusData]    Script Date: 12/01/2026 16:57:20 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO






create procedure [dbo].[pSelect_AnimusData]
@serial int,
@race tinyint
as
if @race > 2
return
select
	case @race
		when 0 then Data0
		when 1 then Data2
		when 2 then Data4
	end as Data0,
	case @race
		when 0 then Data1
		when 1 then Data3
		when 2 then Data5
	end as Data1
from [dbo].[tbl_animusdata] where Serial = @serial







GO
/****** Object:  StoredProcedure [dbo].[pSelect_ArrangeInfo]    Script Date: 12/01/2026 16:57:20 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO








CREATE PROCEDURE [dbo].[pSelect_ArrangeInfo]
@serial int
AS
SELECT 
Arrange
FROM [dbo].[tbl_Base]
WHERE Serial = @serial







GO
/****** Object:  StoredProcedure [dbo].[pselect_automine_inven]    Script Date: 12/01/2026 16:57:20 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO







create procedure [dbo].[pselect_automine_inven]
@nCollisionType tinyint,
@nRace tinyint
AS
select 	top 1 Race, GuildSerial, bWorking, SelectOre, Battery, 
	K_0, O_0, K_1, O_1, K_2, O_2, K_3, O_3, K_4, O_4, K_5, O_5, K_6, O_6, K_7, O_7, k_8, O_8, K_9, O_9,
	K_10, O_10, K_11, O_11, K_12, O_12, K_13, O_13, K_14, O_14, K_15, O_15, K_16, O_16, K_17, O_17, k_18, O_18, K_19, O_19,
	K_20, O_20, K_21, O_21, K_22, O_22, K_23, O_23, K_24, O_24, K_25, O_25, K_26, O_26, K_27, O_27, k_28, O_28, K_29, O_29,
	K_30, O_30, K_31, O_31, K_32, O_32, K_33, O_33, K_34, O_34, K_35, O_35, K_36, O_36, K_37, O_37, k_38, O_38, K_39, O_39,
	K_40, O_40, K_41, O_41, K_42, O_42, K_43, O_43, K_44, O_44, K_45, O_45, K_46, O_46, K_47, O_47, k_48, O_48, K_49, O_49,
	K_50, O_50, K_51, O_51, K_52, O_52, K_53, O_53, K_54, O_54, K_55, O_55, K_56, O_56, K_57, O_57, k_58, O_58, K_59, O_59,
	K_60, O_60, K_61, O_61, K_62, O_62, K_63, O_63, K_64, O_64, K_65, O_65, K_66, O_66, K_67, O_67, k_68, O_68, K_69, O_69,
	K_70, O_70, K_71, O_71, K_72, O_72, K_73, O_73, K_74, O_74, K_75, O_75, K_76, O_76, K_77, O_77, k_78, O_78, K_79, O_79
from [dbo].[tbl_automine_inven]
where dck = 0 and CollisionType = @nCollisionType and Race = @nRace
order by serial desc







GO
/****** Object:  StoredProcedure [dbo].[pSelect_Base]    Script Date: 12/01/2026 16:57:20 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO






/****** Object:  Stored Procedure dbo.pSelect_Base    Script Date: 2003-09-27 오후 3:22:14 ******/
CREATE PROCEDURE [dbo].[pSelect_Base]
@serial int
AS
SELECT *
FROM [dbo].[tbl_base]
WHERE Serial = @serial  AND DCK =  0







GO
/****** Object:  StoredProcedure [dbo].[pSelect_BossAccWinRate]    Script Date: 12/01/2026 16:57:20 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO



CREATE PROCEDURE [dbo].[pSelect_BossAccWinRate]
@race tinyint,
@bossserial int,
@worldname varchar(32)
AS

set nocount on

declare @totalcnt int
declare @wincnt int

set @totalcnt = 0
set @wincnt = 0

if @race=0
	begin
	select @totalcnt = count([bossserial0]) from [dbo].[tbl_racebattle_log] where [bossserial0]=@bossserial
	select @wincnt = count([bossserial0]) from [dbo].[tbl_racebattle_log] where [bossserial0]=@bossserial and [winrace]=@race
	end
else if @race=1
	begin
	select @totalcnt = count([bossserial1]) from [dbo].[tbl_racebattle_log] where [bossserial1]=@bossserial
	select @wincnt = count([bossserial1]) from [dbo].[tbl_racebattle_log] where [bossserial1]=@bossserial and [winrace]=@race
	end
else
	begin
	select @totalcnt = count([bossserial2]) from [dbo].[tbl_racebattle_log] where [bossserial2]=@bossserial
	select @wincnt = count([bossserial2]) from [dbo].[tbl_racebattle_log] where [bossserial2]=@bossserial and [winrace]=@race
	end

select @totalcnt, @wincnt

set nocount off







GO
/****** Object:  StoredProcedure [dbo].[pSelect_BossWinRate]    Script Date: 12/01/2026 16:57:20 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO



CREATE PROCEDURE [dbo].[pSelect_BossWinRate]
@race tinyint,
@worldname varchar(32),
@date char(8)
AS

set nocount on

declare @totalcnt int
declare @wincnt int

set @totalcnt = 0
set @wincnt = 0

select @totalcnt = count([idx]) from [dbo].[tbl_racebattle_log] where [szdate]>=@date and [worldname]=@worldname
select @wincnt = count([idx]) from [dbo].[tbl_racebattle_log] where [szdate]>=@date and [worldname]=@worldname and [winrace]=@race

select @totalcnt, @wincnt

set nocount off







GO
/****** Object:  StoredProcedure [dbo].[pSelect_Buddy20071220]    Script Date: 12/01/2026 16:57:20 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO

/*
   작성자 : 정우철
   작성일 : 2007-12-20 (목)
   내용    : pSelect_Buddy 프로시져 수정
*/

CREATE procedure [dbo].[pSelect_Buddy20071220]
@serial int
as

select 
serial0,
isnull( ( select case dck
	when 0 then name
	when 1 then deletename
	end COLLATE Chinese_Simplified_Stroke_Order_100_CI_AI as name
	from tbl_base where serial = serial0),N'*')  as name0,
serial1,
isnull( ( select case dck
	when 0 then name
	when 1 then deletename
	end COLLATE Chinese_Simplified_Stroke_Order_100_CI_AI as name
	from tbl_base where serial = serial1),N'*')  as name1,
serial2,
isnull( ( select case dck
	when 0 then name
	when 1 then deletename
	end COLLATE Chinese_Simplified_Stroke_Order_100_CI_AI as name
	from tbl_base where serial = serial2),N'*')  as name2,
serial3,
isnull( ( select case dck
	when 0 then name
	when 1 then deletename
	end COLLATE Chinese_Simplified_Stroke_Order_100_CI_AI as name
	from tbl_base where serial = serial3),N'*')  as name3,
serial4,
isnull( ( select case dck
	when 0 then name
	when 1 then deletename
	end COLLATE Chinese_Simplified_Stroke_Order_100_CI_AI as name
	from tbl_base where serial = serial4),N'*')  as name4,
serial5,
isnull( ( select case dck
	when 0 then name
	when 1 then deletename
	end COLLATE Chinese_Simplified_Stroke_Order_100_CI_AI as name
	from tbl_base where serial = serial5),N'*')  as name5,
serial6,
isnull( ( select case dck
	when 0 then name
	when 1 then deletename
	end COLLATE Chinese_Simplified_Stroke_Order_100_CI_AI as name
	from tbl_base where serial = serial6),N'*')  as name6,
serial7,
isnull( ( select case dck
	when 0 then name
	when 1 then deletename
	end COLLATE Chinese_Simplified_Stroke_Order_100_CI_AI as name
	from tbl_base where serial = serial7),N'*')  as name7,
serial8,
isnull( ( select case dck
	when 0 then name
	when 1 then deletename
	end COLLATE Chinese_Simplified_Stroke_Order_100_CI_AI as name
	from tbl_base where serial = serial8),N'*')  as name8,
serial9,
isnull( ( select case dck
	when 0 then name
	when 1 then deletename
	end COLLATE Chinese_Simplified_Stroke_Order_100_CI_AI as name
	from tbl_base where serial = serial9),N'*')  as name9,
serial10,
isnull( ( select case dck
	when 0 then name
	when 1 then deletename
	end COLLATE Chinese_Simplified_Stroke_Order_100_CI_AI as name
	from tbl_base where serial = serial10),N'*')  as name10,
serial11,
isnull( ( select case dck
	when 0 then name
	when 1 then deletename
	end COLLATE Chinese_Simplified_Stroke_Order_100_CI_AI as name
	from tbl_base where serial = serial11),N'*')  as name11,
serial12,
isnull( ( select case dck
	when 0 then name
	when 1 then deletename
	end COLLATE Chinese_Simplified_Stroke_Order_100_CI_AI as name
	from tbl_base where serial = serial12),N'*')  as name12,
serial13,
isnull( ( select case dck
	when 0 then name
	when 1 then deletename
	end COLLATE Chinese_Simplified_Stroke_Order_100_CI_AI as name
	from tbl_base where serial = serial13),N'*')  as name13,
serial14,
isnull( ( select case dck
	when 0 then name
	when 1 then deletename
	end COLLATE Chinese_Simplified_Stroke_Order_100_CI_AI as name
	from tbl_base where serial = serial14),N'*')  as name14,
serial15,
isnull( ( select case dck
	when 0 then name
	when 1 then deletename
	end COLLATE Chinese_Simplified_Stroke_Order_100_CI_AI as name
	from tbl_base where serial = serial15),N'*')  as name15,
serial16,
isnull( ( select case dck
	when 0 then name
	when 1 then deletename
	end COLLATE Chinese_Simplified_Stroke_Order_100_CI_AI as name
	from tbl_base where serial = serial16),N'*')  as name16,
serial17,
isnull( ( select case dck
	when 0 then name
	when 1 then deletename
	end COLLATE Chinese_Simplified_Stroke_Order_100_CI_AI as name
	from tbl_base where serial = serial17),N'*')  as name17,
serial18,
isnull( ( select case dck
	when 0 then name
	when 1 then deletename
	end COLLATE Chinese_Simplified_Stroke_Order_100_CI_AI as name
	from tbl_base where serial = serial18),N'*')  as name18,
serial19,
isnull( ( select case dck
	when 0 then name
	when 1 then deletename
	end COLLATE Chinese_Simplified_Stroke_Order_100_CI_AI as name
	from tbl_base where serial = serial19),N'*')  as name19,
serial20,
isnull( ( select case dck
	when 0 then name
	when 1 then deletename
	end COLLATE Chinese_Simplified_Stroke_Order_100_CI_AI as name
	from tbl_base where serial = serial20),N'*')  as name20,
serial21,
isnull( ( select case dck
	when 0 then name
	when 1 then deletename
	end COLLATE Chinese_Simplified_Stroke_Order_100_CI_AI as name
	from tbl_base where serial = serial21),N'*')  as name21,
serial22,
isnull( ( select case dck
	when 0 then name
	when 1 then deletename
	end COLLATE Chinese_Simplified_Stroke_Order_100_CI_AI as name
	from tbl_base where serial = serial22),N'*')  as name22,
serial23,
isnull( ( select case dck
	when 0 then name
	when 1 then deletename
	end COLLATE Chinese_Simplified_Stroke_Order_100_CI_AI as name
	from tbl_base where serial = serial23),N'*')  as name23,
serial24,
isnull( ( select case dck
	when 0 then name
	when 1 then deletename
	end COLLATE Chinese_Simplified_Stroke_Order_100_CI_AI as name
	from tbl_base where serial = serial24),N'*')  as name24,
serial25,
isnull( ( select case dck
	when 0 then name
	when 1 then deletename
	end COLLATE Chinese_Simplified_Stroke_Order_100_CI_AI as name
	from tbl_base where serial = serial25),N'*')  as name25,
serial26,
isnull( ( select case dck
	when 0 then name
	when 1 then deletename
	end COLLATE Chinese_Simplified_Stroke_Order_100_CI_AI as name
	from tbl_base where serial = serial26),N'*')  as name26,
serial27,
isnull( ( select case dck
	when 0 then name
	when 1 then deletename
	end COLLATE Chinese_Simplified_Stroke_Order_100_CI_AI as name
	from tbl_base where serial = serial27),N'*')  as name27,
serial28,
isnull( ( select case dck
	when 0 then name
	when 1 then deletename
	end COLLATE Chinese_Simplified_Stroke_Order_100_CI_AI as name
	from tbl_base where serial = serial28),N'*')  as name28,
serial29,
isnull( ( select case dck
	when 0 then name
	when 1 then deletename
	end COLLATE Chinese_Simplified_Stroke_Order_100_CI_AI as name
	from tbl_base where serial = serial29),N'*')  as name29,
serial30,
isnull( ( select case dck
	when 0 then name
	when 1 then deletename
	end COLLATE Chinese_Simplified_Stroke_Order_100_CI_AI as name
	from tbl_base where serial = serial30),N'*')  as name30,
serial31,
isnull( ( select case dck
	when 0 then name
	when 1 then deletename
	end COLLATE Chinese_Simplified_Stroke_Order_100_CI_AI as name
	from tbl_base where serial = serial31),N'*')  as name31,
serial32,
isnull( ( select case dck
	when 0 then name
	when 1 then deletename
	end COLLATE Chinese_Simplified_Stroke_Order_100_CI_AI as name
	from tbl_base where serial = serial32),N'*')  as name32,
serial33,
isnull( ( select case dck
	when 0 then name
	when 1 then deletename
	end COLLATE Chinese_Simplified_Stroke_Order_100_CI_AI as name
	from tbl_base where serial = serial33),N'*')  as name33,
serial34,
isnull( ( select case dck
	when 0 then name
	when 1 then deletename
	end COLLATE Chinese_Simplified_Stroke_Order_100_CI_AI as name
	from tbl_base where serial = serial34),N'*')  as name34,
serial35,
isnull( ( select case dck
	when 0 then name
	when 1 then deletename
	end COLLATE Chinese_Simplified_Stroke_Order_100_CI_AI as name
	from tbl_base where serial = serial35),N'*')  as name35,
serial36,
isnull( ( select case dck
	when 0 then name
	when 1 then deletename
	end COLLATE Chinese_Simplified_Stroke_Order_100_CI_AI as name
	from tbl_base where serial = serial36),N'*')  as name36,
serial37,
isnull( ( select case dck
	when 0 then name
	when 1 then deletename
	end COLLATE Chinese_Simplified_Stroke_Order_100_CI_AI as name
	from tbl_base where serial = serial37),N'*')  as name37,
serial38,
isnull( ( select case dck
	when 0 then name
	when 1 then deletename
	end COLLATE Chinese_Simplified_Stroke_Order_100_CI_AI as name
	from tbl_base where serial = serial38),N'*')  as name38,
serial39,
isnull( ( select case dck
	when 0 then name
	when 1 then deletename
	end COLLATE Chinese_Simplified_Stroke_Order_100_CI_AI as name
	from tbl_base where serial = serial39),N'*')  as name39,
serial40,
isnull( ( select case dck
	when 0 then name
	when 1 then deletename
	end COLLATE Chinese_Simplified_Stroke_Order_100_CI_AI as name
	from tbl_base where serial = serial40),N'*')  as name40,
serial41,
isnull( ( select case dck
	when 0 then name
	when 1 then deletename
	end COLLATE Chinese_Simplified_Stroke_Order_100_CI_AI as name
	from tbl_base where serial = serial41),N'*')  as name41,
serial42,
isnull( ( select case dck
	when 0 then name
	when 1 then deletename
	end COLLATE Chinese_Simplified_Stroke_Order_100_CI_AI as name
	from tbl_base where serial = serial42),N'*')  as name42,
serial43,
isnull( ( select case dck
	when 0 then name
	when 1 then deletename
	end COLLATE Chinese_Simplified_Stroke_Order_100_CI_AI as name
	from tbl_base where serial = serial43),N'*')  as name43,
serial44,
isnull( ( select case dck
	when 0 then name
	when 1 then deletename
	end COLLATE Chinese_Simplified_Stroke_Order_100_CI_AI as name
	from tbl_base where serial = serial44),N'*')  as name44,
serial45,
isnull( ( select case dck
	when 0 then name
	when 1 then deletename
	end COLLATE Chinese_Simplified_Stroke_Order_100_CI_AI as name
	from tbl_base where serial = serial45),N'*')  as name45,
serial46,
isnull( ( select case dck
	when 0 then name
	when 1 then deletename
	end COLLATE Chinese_Simplified_Stroke_Order_100_CI_AI as name
	from tbl_base where serial = serial46),N'*')  as name46,
serial47,
isnull( ( select case dck
	when 0 then name
	when 1 then deletename
	end COLLATE Chinese_Simplified_Stroke_Order_100_CI_AI as name
	from tbl_base where serial = serial47),N'*')  as name47,
serial48,
isnull( ( select case dck
	when 0 then name
	when 1 then deletename
	end COLLATE Chinese_Simplified_Stroke_Order_100_CI_AI as name
	from tbl_base where serial = serial48),N'*')  as name48,
serial49,
isnull( ( select case dck
	when 0 then name
	when 1 then deletename
	end COLLATE Chinese_Simplified_Stroke_Order_100_CI_AI as name
	from tbl_base where serial = serial49),N'*')  as name49

from tbl_buddy where serial = @serial





GO
/****** Object:  StoredProcedure [dbo].[pSelect_Cash_LimSale]    Script Date: 12/01/2026 16:57:20 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO

CREATE     PROCEDURE [dbo].[pSelect_Cash_LimSale]  
AS  
select  
DCK,  
LimSaleNum,  
Code_K0, Num0,  
Code_K1, Num1,  
Code_K2, Num2,  
Code_K3, Num3,  
Code_K4, Num4,  
Code_K5, Num5,  
Code_K6, Num6,  
Code_K7, Num7,  
Code_K8, Num8,  
Code_K9, Num9,  
Code_K10, Num10,  
Code_K11, Num11,  
Code_K12, Num12,  
Code_K13, Num13,  
Code_K14, Num14,  
Code_K15, Num15,  
Code_K16, Num16,  
Code_K17, Num17,  
Code_K18, Num18,  
Code_K19, Num19  
from tbl_Cash_LimSale  
  




GO
/****** Object:  StoredProcedure [dbo].[pSelect_CharacterBaseInfo]    Script Date: 12/01/2026 16:57:20 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO








CREATE PROCEDURE [dbo].[pSelect_CharacterBaseInfo]
@serial int
AS
SELECT Name, Slot, Race, Class, Lv, 
	Dalant, Gold, BaseShape, LastConnTime,
	EK0, EK1, EK2, EK3, EK4, EK5, EK6, EK7, 
	EU0, EU1, EU2, EU3, EU4, EU5, EU6, EU7, FirstConnTime
	from [dbo].[tbl_base] where Serial=@serial and DCK = 0







GO
/****** Object:  StoredProcedure [dbo].[pSelect_CharacterBaseInfo_20061115]    Script Date: 12/01/2026 16:57:20 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO









CREATE PROCEDURE [dbo].[pSelect_CharacterBaseInfo_20061115]
@serial int
AS
SELECT Name, Slot, Race, Class, Lv, 
	Dalant, Gold, BaseShape, LastConnTime,
	EK0, EK1, EK2, EK3, EK4, EK5, EK6, EK7, 
	EU0, EU1, EU2, EU3, EU4, EU5, EU6, EU7,
	ES0, ES1, ES2, ES3, ES4, ES5, ES6, ES7,
	ET0, ET1, ET2, ET3, ET4, ET5, ET6, ET7,
	FirstConnTime
	from [dbo].[tbl_base] where Serial=@serial and DCK = 0






GO
/****** Object:  StoredProcedure [dbo].[pSelect_CharacterBaseInfoByCharSerial]    Script Date: 12/01/2026 16:57:20 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO



/*    
캐릭터시리얼로 캐릭터의 Base 정보를 얻어온다    
*/    
CREATE PROCEDURE [dbo].[pSelect_CharacterBaseInfoByCharSerial]    
@characterserial int   
AS    
SELECT Name, Race, Class, Slot, Lv, Dalant,     
 Gold, BaseShape, LastConnTime, Account,     
 EK0, EK1, EK2, EK3, EK4, EK5, EK6, EK7,     
 EU0, EU1, EU2, EU3, EU4, EU5, EU6, EU7    
 from [dbo].[tbl_base] where serial=@characterserial and DCK = 0    






GO
/****** Object:  StoredProcedure [dbo].[pSelect_CharacterBaseInfoByCharSerial_20071023]    Script Date: 12/01/2026 16:57:20 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO


/*    
캐릭터시리얼로 캐릭터의 Base 정보를 얻어온다    
*/    
CREATE PROCEDURE [dbo].[pSelect_CharacterBaseInfoByCharSerial_20071023]    
@characterserial int    
AS    
SELECT Name, Race, Class, Slot, Lv, Dalant,     
 Gold, BaseShape, LastConnTime, Account,     
 EK0, EK1, EK2, EK3, EK4, EK5, EK6, EK7,     
 EU0, EU1, EU2, EU3, EU4, EU5, EU6, EU7,    
 ES0, ES1, ES2, ES3, ES4, ES5, ES6, ES7,    
 ET0, ET1, ET2, ET3, ET4, ET5, ET6, ET7    
 from [dbo].[tbl_base] where serial=@characterserial and DCK = 0    






GO
/****** Object:  StoredProcedure [dbo].[pSelect_CharacterBaseInfoByName]    Script Date: 12/01/2026 16:57:20 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO







/*
캐릭터이름으로 캐릭터의 Base 정보를 얻어온다
*/
CREATE PROCEDURE [dbo].[pSelect_CharacterBaseInfoByName]
@charactername varchar(16)
AS
SELECT Serial, Race, Class, Slot, Lv, Dalant, 
	Gold, BaseShape, LastConnTime, Account, 
	EK0, EK1, EK2, EK3, EK4, EK5, EK6, EK7, 
	EU0, EU1, EU2, EU3, EU4, EU5, EU6, EU7
	from [dbo].[tbl_base] where Name=@charactername and DCK = 0







GO
/****** Object:  StoredProcedure [dbo].[pSelect_CharacterBaseInfoByName_20061115]    Script Date: 12/01/2026 16:57:20 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO








/*
캐릭터이름으로 캐릭터의 Base 정보를 얻어온다
*/
CREATE PROCEDURE [dbo].[pSelect_CharacterBaseInfoByName_20061115]
@charactername varchar(16)
AS
SELECT Serial, Race, Class, Slot, Lv, Dalant, 
	Gold, BaseShape, LastConnTime, Account, 
	EK0, EK1, EK2, EK3, EK4, EK5, EK6, EK7, 
	EU0, EU1, EU2, EU3, EU4, EU5, EU6, EU7,
	ES0, ES1, ES2, ES3, ES4, ES5, ES6, ES7,
	ET0, ET1, ET2, ET3, ET4, ET5, ET6, ET7
	from [dbo].[tbl_base] where Name=@charactername and DCK = 0






GO
/****** Object:  StoredProcedure [dbo].[pSelect_CharacterBaseInfoBySerial]    Script Date: 12/01/2026 16:57:20 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO







/*
계정Serial번호로 캐릭터의 Base 정보를 얻어온다
*/
CREATE PROCEDURE [dbo].[pSelect_CharacterBaseInfoBySerial]
@accountserial int
AS
SELECT top 3 Serial, Name, Slot, Race, Class, Lv, 
	Dalant, Gold, BaseShape, LastConnTime,
	EK0, EK1, EK2, EK3, EK4, EK5, EK6, EK7, 
	EU0, EU1, EU2, EU3, EU4, EU5, EU6, EU7
	from [dbo].[tbl_base] where AccountSerial=@accountserial and DCK = 0






GO
/****** Object:  StoredProcedure [dbo].[pSelect_CharacterBaseInfoBySerial_20061115]    Script Date: 12/01/2026 16:57:20 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO








/*
계정Serial번호로 캐릭터의 Base 정보를 얻어온다
*/
CREATE PROCEDURE [dbo].[pSelect_CharacterBaseInfoBySerial_20061115]
@accountserial int
AS
SELECT Serial, Name, Slot, Race, Class, Lv, 
	Dalant, Gold, BaseShape, LastConnTime,
	EK0, EK1, EK2, EK3, EK4, EK5, EK6, EK7, 
	EU0, EU1, EU2, EU3, EU4, EU5, EU6, EU7,
	ES0, ES1, ES2, ES3, ES4, ES5, ES6, ES7,
	ET0, ET1, ET2, ET3, ET4, ET5, ET6, ET7
	from [dbo].[tbl_base] where AccountSerial=@accountserial and DCK = 0






GO
/****** Object:  StoredProcedure [dbo].[pSelect_CharacterGeneralInfo_20061115]    Script Date: 12/01/2026 16:57:20 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO









--drop procedure [dbo].[pSelect_CharacterGeneralInfo_20061115]
CREATE  PROCEDURE [dbo].[pSelect_CharacterGeneralInfo_20061115]
@characterSerial int
AS
SELECT HP, FP, SP, Exp, LossExp, BagNum, Map, X, Y, Z, TotalPlayMin, LeftResList, 
	EK0, ED0, ES0, ET0,
	EK1, ED1, ES1, ET1,
	EK2, ED2, ES2, ET2,
	EK3, ED3, ES3, ET3,
	EK4, ED4, ES4, ET4,
	EK5, ED5, ES5, ET5,
/*
	EK0, EK1, EK2, EK3, EK4, EK5,
	ED0, ED1, ED2, ED3, ED4, ED5,
	ES0, ES1, ES2, ES3, ES4, ES5,
	ET0, ET1, ET2, ET3, ET4, ET5,
*/
	F0, FS0, F1, FS1, F2, FS2, F3, FS3, F4, FS4, F5, FS5, F6, FS6, F7, FS7, F8, FS8, F9, FS9,
	F10, FS10, F11, FS11, F12, FS12, F13, FS13, F14, FS14, F15, FS15, F16, FS16, F17, FS17, F18, FS18, F19, FS19,
	F20, FS20, F21, FS21, F22, FS22, F23, FS23, F24, FS24, F25, FS25, F26, FS26, F27, FS27, F28, FS28, F29, FS29,
	F30, FS30, F31, FS31, F32, FS32, F33, FS33, F34, FS34, F35, FS35, F36, FS36, F37, FS37, F38, FS38, F39, FS39,
	F40, FS40, F41, FS41, F42, FS42, F43, FS43, F44, FS44, F45, FS45, F46, FS46, F47, FS47, F48, FS48, F49, FS49,
	F50, FS50, F51, FS51, F52, FS52, F53, FS53, F54, FS54, F55, FS55, F56, FS56, F57, FS57, F58, FS58, F59, FS59,
	F60, FS60, F61, FS61, F62, FS62, F63, FS63, F64, FS64, F65, FS65, F66, FS66, F67, FS67, F68, FS68, F69, FS69,
	F70, FS70, F71, FS71, F72, FS72, F73, FS73, F74, FS74, F75, FS75, F76, FS76, F77, FS77, F78, FS78, F79, FS79,
	F80, FS80, F81, FS81, F82, FS82, F83, FS83, F84, FS84, F85, FS85, F86, FS86, F87, FS87,--, F88, FS88, F89, FS89,
	--F90, FS90, F91, FS91, F92, FS92, F93, FS93, F94, FS94, F95, FS95, F96, FS96, F97, FS97, F98, FS98, F99, FS99,

/*
	F10, F11, F12, F13, F14, F15, F16, F17, F18, F19, 
	F20, F21, F22, F23, F24, F25, F26, F27, F28, F29, 
	F30, F31, F32, F33, F34, F35, F36, F37, F38, F39, 
	F40, F41, F42, F43, F44, F45, F46, F47, F48, F49, 
	F50, F51, F52, F53, F54, F55, F56, F57, F58, F59, 
	F60, F61, F62, F63, F64, F65, F66, F67, F68, F69, 
	F70, F71, F72, F73, F74, F75, F76, F77, F78, F79, 
	F80, F81, F82, F83, F84, F85, F86, F87, 
*/

	WM0, WM1,
	FM0, FM1, FM2, FM3, FM4, FM5, FM6, FM7, FM8, FM9, 
	FM10, FM11, FM12, FM13, FM14, FM15, FM16, FM17, FM18, FM19, 
	FM20, FM21, FM22, FM23,
	SM0, SM1, SM2, SM3, SM4, SM5, SM6, SM7, SM8, SM9, 
	SM10, SM11, SM12, SM13, SM14, SM15, SM16, SM17, SM18, SM19, 
	SM20, SM21, SM22, SM23, SM24, SM25, SM26, SM27, SM28, SM29,
	SM30, SM31, SM32, SM33, SM34, SM35, SM36, SM37, SM38, SM39, 
	SM40, SM41, SM42, SM43, SM44, SM45, SM46, SM47,
	MI0, MI1, MI2,
	SR, DM, PM,

	AK0, AD0, AP0, AS0,
	AK1, AD1, AP1, AS1,
	AK2, AD2, AP2, AS2,
	AK3, AD3, AP3, AS3,	
/*
	AK0, AK1, AK2, AK3,
	AD0, AD1, AD2, AD3,
	AP0, AP1, AP2, AP3,
*/
	Class0, Class1, Class2,
	ClassInitCnt, LastClassGrade,
	PvpPoint, PvpCashBag,
	BindMapCode, BindDummy,
	GuildSerial, GuildGrade, DP, RDDelay,
	TakeLastMentalTicket, 					-- milksik : 채굴권 ( 홀리맨탈 티켓 )
	TakeLastCriTicket					-- milksik : 채굴권 ( 크리쟁 티켓 )
from [dbo].[tbl_general] where serial=@characterSerial and DCK = 0






GO
/****** Object:  StoredProcedure [dbo].[pSelect_CharacterGeneralInfo_20061221]    Script Date: 12/01/2026 16:57:20 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO



CREATE  PROCEDURE [dbo].[pSelect_CharacterGeneralInfo_20061221]
@characterSerial int
AS
SELECT HP, FP, SP, Exp, LossExp, MaxLevel, BagNum, Map, X, Y, Z, TotalPlayMin, LeftResList, 		-- wanee : MaxLevel ( 최대제한레벨 )
	EK0, ED0, ES0, ET0,
	EK1, ED1, ES1, ET1,
	EK2, ED2, ES2, ET2,
	EK3, ED3, ES3, ET3,
	EK4, ED4, ES4, ET4,
	EK5, ED5, ES5, ET5,
/*
	EK0, EK1, EK2, EK3, EK4, EK5,
	ED0, ED1, ED2, ED3, ED4, ED5,
	ES0, ES1, ES2, ES3, ES4, ES5,
	ET0, ET1, ET2, ET3, ET4, ET5,
*/
	F0, FS0, F1, FS1, F2, FS2, F3, FS3, F4, FS4, F5, FS5, F6, FS6, F7, FS7, F8, FS8, F9, FS9,
	F10, FS10, F11, FS11, F12, FS12, F13, FS13, F14, FS14, F15, FS15, F16, FS16, F17, FS17, F18, FS18, F19, FS19,
	F20, FS20, F21, FS21, F22, FS22, F23, FS23, F24, FS24, F25, FS25, F26, FS26, F27, FS27, F28, FS28, F29, FS29,
	F30, FS30, F31, FS31, F32, FS32, F33, FS33, F34, FS34, F35, FS35, F36, FS36, F37, FS37, F38, FS38, F39, FS39,
	F40, FS40, F41, FS41, F42, FS42, F43, FS43, F44, FS44, F45, FS45, F46, FS46, F47, FS47, F48, FS48, F49, FS49,
	F50, FS50, F51, FS51, F52, FS52, F53, FS53, F54, FS54, F55, FS55, F56, FS56, F57, FS57, F58, FS58, F59, FS59,
	F60, FS60, F61, FS61, F62, FS62, F63, FS63, F64, FS64, F65, FS65, F66, FS66, F67, FS67, F68, FS68, F69, FS69,
	F70, FS70, F71, FS71, F72, FS72, F73, FS73, F74, FS74, F75, FS75, F76, FS76, F77, FS77, F78, FS78, F79, FS79,
	F80, FS80, F81, FS81, F82, FS82, F83, FS83, F84, FS84, F85, FS85, F86, FS86, F87, FS87,--, F88, FS88, F89, FS89,
	--F90, FS90, F91, FS91, F92, FS92, F93, FS93, F94, FS94, F95, FS95, F96, FS96, F97, FS97, F98, FS98, F99, FS99,

/*
	F10, F11, F12, F13, F14, F15, F16, F17, F18, F19, 
	F20, F21, F22, F23, F24, F25, F26, F27, F28, F29, 
	F30, F31, F32, F33, F34, F35, F36, F37, F38, F39, 
	F40, F41, F42, F43, F44, F45, F46, F47, F48, F49, 
	F50, F51, F52, F53, F54, F55, F56, F57, F58, F59, 
	F60, F61, F62, F63, F64, F65, F66, F67, F68, F69, 
	F70, F71, F72, F73, F74, F75, F76, F77, F78, F79, 
	F80, F81, F82, F83, F84, F85, F86, F87, 
*/

	WM0, WM1,
	FM0, FM1, FM2, FM3, FM4, FM5, FM6, FM7, FM8, FM9, 
	FM10, FM11, FM12, FM13, FM14, FM15, FM16, FM17, FM18, FM19, 
	FM20, FM21, FM22, FM23,
	SM0, SM1, SM2, SM3, SM4, SM5, SM6, SM7, SM8, SM9, 
	SM10, SM11, SM12, SM13, SM14, SM15, SM16, SM17, SM18, SM19, 
	SM20, SM21, SM22, SM23, SM24, SM25, SM26, SM27, SM28, SM29,
	SM30, SM31, SM32, SM33, SM34, SM35, SM36, SM37, SM38, SM39, 
	SM40, SM41, SM42, SM43, SM44, SM45, SM46, SM47,
	MI0, MI1, MI2,
	SR, DM, PM,

	AK0, AD0, AP0, AS0,
	AK1, AD1, AP1, AS1,
	AK2, AD2, AP2, AS2,
	AK3, AD3, AP3, AS3,	
/*
	AK0, AK1, AK2, AK3,
	AD0, AD1, AD2, AD3,
	AP0, AP1, AP2, AP3,
*/
	Class0, Class1, Class2,
	ClassInitCnt, LastClassGrade,
	PvpPoint, PvpCashBag,
	BindMapCode, BindDummy,
	GuildSerial, GuildGrade, DP, RDDelay,
	TakeLastMentalTicket, 					-- milksik : 채굴권 ( 홀리맨탈 티켓 )
	TakeLastCriTicket					-- milksik : 채굴권 ( 크리쟁 티켓 )
from [dbo].[tbl_general] where serial=@characterSerial and DCK = 0






GO
/****** Object:  StoredProcedure [dbo].[pSelect_CharacterGeneralInfo_20070614]    Script Date: 12/01/2026 16:57:20 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO




/*
   작성자 : 김태완
   작성일 : 2007-06-14 (목)
   내용    : 길드내등급 GuildGrade 필드에서 GuildStatus 필드로 변경(길드위원회 축소 관련)
*/
CREATE  PROCEDURE [dbo].[pSelect_CharacterGeneralInfo_20070614]
@characterSerial int
AS
SELECT HP, FP, SP, Exp, LossExp, MaxLevel, BagNum, Map, X, Y, Z, TotalPlayMin, LeftResList, 		-- wanee : MaxLevel ( 최대제한레벨 )
	EK0, ED0, ES0, ET0,
	EK1, ED1, ES1, ET1,
	EK2, ED2, ES2, ET2,
	EK3, ED3, ES3, ET3,
	EK4, ED4, ES4, ET4,
	EK5, ED5, ES5, ET5,
/*
	EK0, EK1, EK2, EK3, EK4, EK5,
	ED0, ED1, ED2, ED3, ED4, ED5,
	ES0, ES1, ES2, ES3, ES4, ES5,
	ET0, ET1, ET2, ET3, ET4, ET5,
*/
	F0, FS0, F1, FS1, F2, FS2, F3, FS3, F4, FS4, F5, FS5, F6, FS6, F7, FS7, F8, FS8, F9, FS9,
	F10, FS10, F11, FS11, F12, FS12, F13, FS13, F14, FS14, F15, FS15, F16, FS16, F17, FS17, F18, FS18, F19, FS19,
	F20, FS20, F21, FS21, F22, FS22, F23, FS23, F24, FS24, F25, FS25, F26, FS26, F27, FS27, F28, FS28, F29, FS29,
	F30, FS30, F31, FS31, F32, FS32, F33, FS33, F34, FS34, F35, FS35, F36, FS36, F37, FS37, F38, FS38, F39, FS39,
	F40, FS40, F41, FS41, F42, FS42, F43, FS43, F44, FS44, F45, FS45, F46, FS46, F47, FS47, F48, FS48, F49, FS49,
	F50, FS50, F51, FS51, F52, FS52, F53, FS53, F54, FS54, F55, FS55, F56, FS56, F57, FS57, F58, FS58, F59, FS59,
	F60, FS60, F61, FS61, F62, FS62, F63, FS63, F64, FS64, F65, FS65, F66, FS66, F67, FS67, F68, FS68, F69, FS69,
	F70, FS70, F71, FS71, F72, FS72, F73, FS73, F74, FS74, F75, FS75, F76, FS76, F77, FS77, F78, FS78, F79, FS79,
	F80, FS80, F81, FS81, F82, FS82, F83, FS83, F84, FS84, F85, FS85, F86, FS86, F87, FS87,--, F88, FS88, F89, FS89,
	--F90, FS90, F91, FS91, F92, FS92, F93, FS93, F94, FS94, F95, FS95, F96, FS96, F97, FS97, F98, FS98, F99, FS99,

/*
	F10, F11, F12, F13, F14, F15, F16, F17, F18, F19, 
	F20, F21, F22, F23, F24, F25, F26, F27, F28, F29, 
	F30, F31, F32, F33, F34, F35, F36, F37, F38, F39, 
	F40, F41, F42, F43, F44, F45, F46, F47, F48, F49, 
	F50, F51, F52, F53, F54, F55, F56, F57, F58, F59, 
	F60, F61, F62, F63, F64, F65, F66, F67, F68, F69, 
	F70, F71, F72, F73, F74, F75, F76, F77, F78, F79, 
	F80, F81, F82, F83, F84, F85, F86, F87, 
*/

	WM0, WM1,
	FM0, FM1, FM2, FM3, FM4, FM5, FM6, FM7, FM8, FM9, 
	FM10, FM11, FM12, FM13, FM14, FM15, FM16, FM17, FM18, FM19, 
	FM20, FM21, FM22, FM23,
	SM0, SM1, SM2, SM3, SM4, SM5, SM6, SM7, SM8, SM9, 
	SM10, SM11, SM12, SM13, SM14, SM15, SM16, SM17, SM18, SM19, 
	SM20, SM21, SM22, SM23, SM24, SM25, SM26, SM27, SM28, SM29,
	SM30, SM31, SM32, SM33, SM34, SM35, SM36, SM37, SM38, SM39, 
	SM40, SM41, SM42, SM43, SM44, SM45, SM46, SM47,
	MI0, MI1, MI2,
	SR, DM, PM,

	AK0, AD0, AP0, AS0,
	AK1, AD1, AP1, AS1,
	AK2, AD2, AP2, AS2,
	AK3, AD3, AP3, AS3,	
/*
	AK0, AK1, AK2, AK3,
	AD0, AD1, AD2, AD3,
	AP0, AP1, AP2, AP3,
*/
	Class0, Class1, Class2,
	ClassInitCnt, LastClassGrade,
	PvpPoint, PvpCashBag,
	BindMapCode, BindDummy,
	GuildSerial, GuildStatus, DP, RDDelay,		-- wanee : 길드내등급 GuildGrade 필드에서 GuildStatus 필드로 변경(길드위원회 축소 관련)
	TakeLastMentalTicket, 				-- milksik : 채굴권 ( 홀리맨탈 티켓 )
	TakeLastCriTicket					-- milksik : 채굴권 ( 크리쟁 티켓 )
from [dbo].[tbl_general] where serial=@characterSerial and DCK = 0





GO
/****** Object:  StoredProcedure [dbo].[pSelect_CharacterGeneralInfo_20081216]    Script Date: 12/01/2026 16:57:20 ******/
SET ANSI_NULLS OFF
GO
SET QUOTED_IDENTIFIER ON
GO

/*
   АЫјєАЪ : °шјцБш
   АЫјєАП : 2008-12-16
   і»їл    : tbl_supplement ЕЧАМєнїЎ ГЯ°ЎИ®АеµИ АеЅД№° ЅЅ·ФА» ·Оµе
*/

CREATE  PROCEDURE [dbo].[pSelect_CharacterGeneralInfo_20081216]
@characterSerial int
AS
SELECT HP, FP, SP, Exp, LossExp, MaxLevel, BagNum, Map, X, Y, Z, TotalPlayMin, LeftResList, 		-- wanee : MaxLevel ( ГЦґлБ¦ЗС·№є§ )
	EK0, ED0, ES0, ET0,
	EK1, ED1, ES1, ET1,
	EK2, ED2, ES2, ET2,
	EK3, ED3, ES3, ET3,
	EK4, ED4, ES4, ET4,
	EK5, ED5, ES5, ET5,

(SELECT EK6 from [dbo].[tbl_supplement] where serial=@characterSerial and DCK = 0 ) as EK6 ,
(SELECT ED6 from [dbo].[tbl_supplement] where serial=@characterSerial and DCK = 0 ) as ED6 ,
(SELECT ES6 from [dbo].[tbl_supplement] where serial=@characterSerial and DCK = 0 ) as ES6 ,
(SELECT ET6 from [dbo].[tbl_supplement] where serial=@characterSerial and DCK = 0 ) as ET6 ,


/*
	EK0, EK1, EK2, EK3, EK4, EK5, EK6,
	ED0, ED1, ED2, ED3, ED4, ED5, ED6, 
	ES0, ES1, ES2, ES3, ES4, ES5,
	ET0, ET1, ET2, ET3, ET4, ET5,

*/
	F0, FS0, F1, FS1, F2, FS2, F3, FS3, F4, FS4, F5, FS5, F6, FS6, F7, FS7, F8, FS8, F9, FS9,
	F10, FS10, F11, FS11, F12, FS12, F13, FS13, F14, FS14, F15, FS15, F16, FS16, F17, FS17, F18, FS18, F19, FS19,
	F20, FS20, F21, FS21, F22, FS22, F23, FS23, F24, FS24, F25, FS25, F26, FS26, F27, FS27, F28, FS28, F29, FS29,
	F30, FS30, F31, FS31, F32, FS32, F33, FS33, F34, FS34, F35, FS35, F36, FS36, F37, FS37, F38, FS38, F39, FS39,
	F40, FS40, F41, FS41, F42, FS42, F43, FS43, F44, FS44, F45, FS45, F46, FS46, F47, FS47, F48, FS48, F49, FS49,
	F50, FS50, F51, FS51, F52, FS52, F53, FS53, F54, FS54, F55, FS55, F56, FS56, F57, FS57, F58, FS58, F59, FS59,
	F60, FS60, F61, FS61, F62, FS62, F63, FS63, F64, FS64, F65, FS65, F66, FS66, F67, FS67, F68, FS68, F69, FS69,
	F70, FS70, F71, FS71, F72, FS72, F73, FS73, F74, FS74, F75, FS75, F76, FS76, F77, FS77, F78, FS78, F79, FS79,
	F80, FS80, F81, FS81, F82, FS82, F83, FS83, F84, FS84, F85, FS85, F86, FS86, F87, FS87,--, F88, FS88, F89, FS89,
	--F90, FS90, F91, FS91, F92, FS92, F93, FS93, F94, FS94, F95, FS95, F96, FS96, F97, FS97, F98, FS98, F99, FS99,

/*
	F10, F11, F12, F13, F14, F15, F16, F17, F18, F19, 
	F20, F21, F22, F23, F24, F25, F26, F27, F28, F29, 
	F30, F31, F32, F33, F34, F35, F36, F37, F38, F39, 
	F40, F41, F42, F43, F44, F45, F46, F47, F48, F49, 
	F50, F51, F52, F53, F54, F55, F56, F57, F58, F59, 
	F60, F61, F62, F63, F64, F65, F66, F67, F68, F69, 
	F70, F71, F72, F73, F74, F75, F76, F77, F78, F79, 
	F80, F81, F82, F83, F84, F85, F86, F87, 
*/

	WM0, WM1,
	FM0, FM1, FM2, FM3, FM4, FM5, FM6, FM7, FM8, FM9, 
	FM10, FM11, FM12, FM13, FM14, FM15, FM16, FM17, FM18, FM19, 
	FM20, FM21, FM22, FM23,
	SM0, SM1, SM2, SM3, SM4, SM5, SM6, SM7, SM8, SM9, 
	SM10, SM11, SM12, SM13, SM14, SM15, SM16, SM17, SM18, SM19, 
	SM20, SM21, SM22, SM23, SM24, SM25, SM26, SM27, SM28, SM29,
	SM30, SM31, SM32, SM33, SM34, SM35, SM36, SM37, SM38, SM39, 
	SM40, SM41, SM42, SM43, SM44, SM45, SM46, SM47,
	MI0, MI1, MI2,
	SR, DM, PM,

	AK0, AD0, AP0, AS0,
	AK1, AD1, AP1, AS1,
	AK2, AD2, AP2, AS2,
	AK3, AD3, AP3, AS3,	
/*
	AK0, AK1, AK2, AK3,
	AD0, AD1, AD2, AD3,
	AP0, AP1, AP2, AP3,
*/
	Class0, Class1, Class2,
	ClassInitCnt, LastClassGrade,
	PvpPoint, PvpCashBag,
	BindMapCode, BindDummy,
	GuildSerial, GuildStatus, DP, RDDelay,			-- wanee : ±жµеі»µо±Ю GuildGrade ЗКµеїЎј­ GuildStatus ЗКµе·О єЇ°ж(±жµеА§їшИё ГајТ °ь·Г)
	TakeLastMentalTicket, 				-- milksik : Г¤±ј±З ( И¦ё®ёЗЕ» ЖјДП )
	TakeLastCriTicket					-- milksik : Г¤±ј±З ( Е©ё®Ап ЖјДП )
from [dbo].[tbl_general] where serial=@characterSerial and DCK = 0
GO
/****** Object:  StoredProcedure [dbo].[pSelect_CharacterGeneralInfo0614]    Script Date: 12/01/2026 16:57:20 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO








--캐릭터의 General 정보를 얻어온다
CREATE PROCEDURE [dbo].[pSelect_CharacterGeneralInfo0614]
@characterSerial int
AS
SELECT HP, FP, SP, Exp, LossExp, BagNum, Map, X, Y, Z, TotalPlayMin, LeftResList, 
	EK0, EK1, EK2, EK3, EK4, EK5,
	ED0, ED1, ED2, ED3, ED4, ED5,
	F0, F1, F2, F3, F4, F5, F6, F7, F8, F9, 
	F10, F11, F12, F13, F14, F15, F16, F17, F18, F19, 
	F20, F21, F22, F23, F24, F25, F26, F27, F28, F29, 
	F30, F31, F32, F33, F34, F35, F36, F37, F38, F39, 
	F40, F41, F42, F43, F44, F45, F46, F47, F48, F49, 
	F50, F51, F52, F53, F54, F55, F56, F57, F58, F59, 
	F60, F61, F62, F63, F64, F65, F66, F67, F68, F69, 
	F70, F71, F72, F73, F74, F75, F76, F77, F78, F79, 
	F80, F81, F82, F83, F84, F85, F86, F87, 
	WM0, WM1,
	FM0, FM1, FM2, FM3, FM4, FM5, FM6, FM7, FM8, FM9, 
	FM10, FM11, FM12, FM13, FM14, FM15, FM16, FM17, FM18, FM19, 
	FM20, FM21, FM22, FM23,
	SM0, SM1, SM2, SM3, SM4, SM5, SM6, SM7, SM8, SM9, 
	SM10, SM11, SM12, SM13, SM14, SM15, SM16, SM17, SM18, SM19, 
	SM20, SM21, SM22, SM23, SM24, SM25, SM26, SM27, SM28, SM29,
	SM30, SM31, SM32, SM33, SM34, SM35, SM36, SM37, SM38, SM39, 
	SM40, SM41, SM42, SM43, SM44, SM45, SM46, SM47,
	MI0, MI1, MI2,
	SR, DM, PM,
	AK0, AK1, AK2, AK3,
	AD0, AD1, AD2, AD3,
	AP0, AP1, AP2, AP3,
	Class0, Class1, Class2,
	ClassInitCnt, LastClassGrade,
	PvpPoint, PvpCashBag,
	BindMapCode, BindDummy,
	GuildSerial, GuildGrade, DP
from [dbo].[tbl_general] where serial=@characterSerial and DCK = 0







GO
/****** Object:  StoredProcedure [dbo].[pSelect_CharacterGeneralInfo0720]    Script Date: 12/01/2026 16:57:20 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO




CREATE PROCEDURE [dbo].[pSelect_CharacterGeneralInfo0720]
@characterSerial int
AS
SELECT HP, FP, SP, Exp, LossExp, BagNum, Map, X, Y, Z, TotalPlayMin, LeftResList, 
	EK0, EK1, EK2, EK3, EK4, EK5,
	ED0, ED1, ED2, ED3, ED4, ED5,
	F0, F1, F2, F3, F4, F5, F6, F7, F8, F9, 
	F10, F11, F12, F13, F14, F15, F16, F17, F18, F19, 
	F20, F21, F22, F23, F24, F25, F26, F27, F28, F29, 
	F30, F31, F32, F33, F34, F35, F36, F37, F38, F39, 
	F40, F41, F42, F43, F44, F45, F46, F47, F48, F49, 
	F50, F51, F52, F53, F54, F55, F56, F57, F58, F59, 
	F60, F61, F62, F63, F64, F65, F66, F67, F68, F69, 
	F70, F71, F72, F73, F74, F75, F76, F77, F78, F79, 
	F80, F81, F82, F83, F84, F85, F86, F87, 
	WM0, WM1,
	FM0, FM1, FM2, FM3, FM4, FM5, FM6, FM7, FM8, FM9, 
	FM10, FM11, FM12, FM13, FM14, FM15, FM16, FM17, FM18, FM19, 
	FM20, FM21, FM22, FM23,
	SM0, SM1, SM2, SM3, SM4, SM5, SM6, SM7, SM8, SM9, 
	SM10, SM11, SM12, SM13, SM14, SM15, SM16, SM17, SM18, SM19, 
	SM20, SM21, SM22, SM23, SM24, SM25, SM26, SM27, SM28, SM29,
	SM30, SM31, SM32, SM33, SM34, SM35, SM36, SM37, SM38, SM39, 
	SM40, SM41, SM42, SM43, SM44, SM45, SM46, SM47,
	MI0, MI1, MI2,
	SR, DM, PM,
	AK0, AK1, AK2, AK3,
	AD0, AD1, AD2, AD3,
	AP0, AP1, AP2, AP3,
	Class0, Class1, Class2,
	ClassInitCnt, LastClassGrade,
	PvpPoint, PvpCashBag,
	BindMapCode, BindDummy,
	GuildSerial, GuildGrade, DP, RDDelay
from [dbo].[tbl_general] where serial=@characterSerial and DCK = 0







GO
/****** Object:  StoredProcedure [dbo].[pSelect_CharacterGeneralInfo0912]    Script Date: 12/01/2026 16:57:20 ******/
SET ANSI_NULLS OFF
GO
SET QUOTED_IDENTIFIER ON
GO




-- MILKSIK : 채궐 권한 설정 2006-09-12
CREATE  PROCEDURE [dbo].[pSelect_CharacterGeneralInfo0912]
@characterSerial int
AS
SELECT HP, FP, SP, Exp, LossExp, BagNum, Map, X, Y, Z, TotalPlayMin, LeftResList, 
	EK0, EK1, EK2, EK3, EK4, EK5,
	ED0, ED1, ED2, ED3, ED4, ED5,
	F0, F1, F2, F3, F4, F5, F6, F7, F8, F9, 
	F10, F11, F12, F13, F14, F15, F16, F17, F18, F19, 
	F20, F21, F22, F23, F24, F25, F26, F27, F28, F29, 
	F30, F31, F32, F33, F34, F35, F36, F37, F38, F39, 
	F40, F41, F42, F43, F44, F45, F46, F47, F48, F49, 
	F50, F51, F52, F53, F54, F55, F56, F57, F58, F59, 
	F60, F61, F62, F63, F64, F65, F66, F67, F68, F69, 
	F70, F71, F72, F73, F74, F75, F76, F77, F78, F79, 
	F80, F81, F82, F83, F84, F85, F86, F87, 
	WM0, WM1,
	FM0, FM1, FM2, FM3, FM4, FM5, FM6, FM7, FM8, FM9, 
	FM10, FM11, FM12, FM13, FM14, FM15, FM16, FM17, FM18, FM19, 
	FM20, FM21, FM22, FM23,
	SM0, SM1, SM2, SM3, SM4, SM5, SM6, SM7, SM8, SM9, 
	SM10, SM11, SM12, SM13, SM14, SM15, SM16, SM17, SM18, SM19, 
	SM20, SM21, SM22, SM23, SM24, SM25, SM26, SM27, SM28, SM29,
	SM30, SM31, SM32, SM33, SM34, SM35, SM36, SM37, SM38, SM39, 
	SM40, SM41, SM42, SM43, SM44, SM45, SM46, SM47,
	MI0, MI1, MI2,
	SR, DM, PM,
	AK0, AK1, AK2, AK3,
	AD0, AD1, AD2, AD3,
	AP0, AP1, AP2, AP3,
	Class0, Class1, Class2,
	ClassInitCnt, LastClassGrade,
	PvpPoint, PvpCashBag,
	BindMapCode, BindDummy,
	GuildSerial, GuildGrade, DP, RDDelay,
	TakeLastMentalTicket, 					-- milksik : 채굴권 ( 홀리맨탈 티켓 )
	TakeLastCriTicket					-- milksik : 채굴권 ( 크리쟁 티켓 )
from [dbo].[tbl_general] where serial=@characterSerial and DCK = 0






GO
/****** Object:  StoredProcedure [dbo].[pSelect_CharacterSerial]    Script Date: 12/01/2026 16:57:20 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO







/*
캐릭터의 이름으로 시리얼을 불러온다
*/
CREATE PROCEDURE [dbo].[pSelect_CharacterSerial]
@charactername varchar(16)
AS
SELECT serial FROM [dbo].[tbl_base] WHERE Name = @charactername and dck=0







GO
/****** Object:  StoredProcedure [dbo].[pSelect_CharactersInfo]    Script Date: 12/01/2026 16:57:20 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO






/*
계정시리얼로 캐릭터들의 약식정보를 불러온다
*/
CREATE PROCEDURE [dbo].[pSelect_CharactersInfo]
@accountserial int
AS
SELECT Name, Slot, Race, Lv, Serial, DCK, DeleteName FROM [dbo].[tbl_base]
WHERE 
AccountSerial = @accountserial







GO
/****** Object:  StoredProcedure [dbo].[pSelect_CharNumInWorld]    Script Date: 12/01/2026 16:57:20 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO






create procedure [dbo].[pSelect_CharNumInWorld]
@accountserial int
as
select count(serial) from [dbo].[tbl_base] where accountserial = @accountserial and dck = 0






GO
/****** Object:  StoredProcedure [dbo].[pSelect_CombineEx_Result]    Script Date: 12/01/2026 16:57:20 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO





CREATE PROCEDURE [dbo].[pSelect_CombineEx_Result]
@serial int
AS
SELECT 
IsResult,
CheckKey,
DlgType,
Dalant,
ItemListCount,
ItemSelectCount,
K0,D0,U0,
K1,D1,U1,
K2,D2,U2,
K3,D3,U3,
K4,D4,U4,
K5,D5,U5,
K6,D6,U6,
K7,D7,U7,
K8,D8,U8,
K9,D9,U9,
K10,D10,U10,
K11,D11,U11,
K12,D12,U12,
K13,D13,U13,
K14,D14,U14,
K15,D15,U15,
K16,D16,U16,
K17,D17,U17,
K18,D18,U18,
K19,D19,U19,
K20,D20,U20,
K21,D21,U21,
K22,D22,U22,
K23,D23,U23

FROM [dbo].[tbl_itemcombine_ex_result]
WHERE Serial = @serial







GO
/****** Object:  StoredProcedure [dbo].[pSelect_CombineEx_Result_20071010]    Script Date: 12/01/2026 16:57:20 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO



/****** 개체: 저장 프로시저 아이템조합의 결과정보를 가지고 온다. ******/  
CREATE PROCEDURE [dbo].[pSelect_CombineEx_Result_20071010]  
@serial int  
AS  
SELECT   
IsResult,  
CheckKey,  
DlgType,  
Dalant,  
ItemListCount,  
ItemSelectCount,  
K0,D0,U0,  
K1,D1,U1,  
K2,D2,U2,  
K3,D3,U3,  
K4,D4,U4,  
K5,D5,U5,  
K6,D6,U6,  
K7,D7,U7,  
K8,D8,U8,  
K9,D9,U9,  
K10,D10,U10,  
K11,D11,U11,  
K12,D12,U12,  
K13,D13,U13,  
K14,D14,U14,  
K15,D15,U15,  
K16,D16,U16,  
K17,D17,U17,  
K18,D18,U18,  
K19,D19,U19,  
K20,D20,U20,  
K21,D21,U21,  
K22,D22,U22,  
K23,D23,U23,
resulteffecttype,
resulteffectmsgcode

FROM [dbo].[tbl_itemcombine_ex_result]  
WHERE Serial = @serial





GO
/****** Object:  StoredProcedure [dbo].[pSelect_EndRaceBattleCount]    Script Date: 12/01/2026 16:57:20 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO


/*  
   작성자 : 정우철
   작성일 : 2007-07-24 (화)  
   내용    : 최종 접속 종료 후 끝난 종족전 횟수를 구함
*/  
CREATE PROCEDURE [dbo].[pSelect_EndRaceBattleCount]  
@aserial int                      -- 아바타 시리얼  
  
AS  
  
Select count(idx)  
From tbl_racebattle_log  
Where endtime > (Select LastConnTime From tbl_base Where Serial = @aserial)






GO
/****** Object:  StoredProcedure [dbo].[pSelect_Equal_Name]    Script Date: 12/01/2026 16:57:20 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO






/****** Object:  Stored Procedure dbo.pExist_Avator    Script Date: 2003-06-16 오후 1:25:47 ******/
CREATE PROCEDURE [dbo].[pSelect_Equal_Name]
@name VARCHAR(17)
AS
SELECT Name 
FROM [dbo].[tbl_base]
WHERE Name = @name AND DCK = 0







GO
/****** Object:  StoredProcedure [dbo].[pSelect_Exist_Economy]    Script Date: 12/01/2026 16:57:20 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO







CREATE PROCEDURE [dbo].[pSelect_Exist_Economy]
@dwDate int
AS
select 
B_Dalant, C_Dalant, A_Dalant, 
B_Gold, C_Gold, A_Gold,
ManageValue,
B_MineOre1, C_MineOre1, A_MineOre1,
B_MineOre2, C_MineOre2, A_MineOre2,
B_MineOre3, C_MineOre3, A_MineOre3,
B_CutOre1, C_CutOre1, A_CutOre1,
B_CutOre2, C_CutOre2, A_CutOre2,
B_CutOre3, C_CutOre3, A_CutOre3
from [dbo].[tbl_economy_history] where Serial=@dwDate







GO
/****** Object:  StoredProcedure [dbo].[pSelect_FailRaceBattleCount]    Script Date: 12/01/2026 16:57:20 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO





/*
   작성자 : 김태완
   작성일 : 2007-03-23 (금)
   내용    : 최종 접속 종료 후 종족전 실패 횟수를 조회한다
*/
CREATE PROCEDURE [dbo].[pSelect_FailRaceBattleCount]
@aserial		int	-- 아바타 시리얼

AS

Select	count(*)
From	tbl_racebattle_log
Where	winrace = 0xFF
AND	loserace = 0xFF
AND	endtime > (Select LastConnTime From tbl_base Where Serial = @aserial AND DCK=0 AND Lv >= 25)






GO
/****** Object:  StoredProcedure [dbo].[pSelect_FailRaceBattleCount_20070704]    Script Date: 12/01/2026 16:57:20 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO


/*
   작성자 : 김태완
   작성일 : 2007-07-04 (수)
   내용    : 최종 접속 종료 후 종족전 실패 횟수를 조회한다.(race 파라미터 추가)
*/
CREATE PROCEDURE [dbo].[pSelect_FailRaceBattleCount_20070704]
@aserial	int,	-- 아바타 시리얼
@race	tinyint

AS

Select	count(*)
From	tbl_racebattle_log
Where	winrace <> @race
AND	loserace <> @race
AND	endtime > (Select LastConnTime From tbl_base Where Serial = @aserial AND DCK=0 AND Lv >= 25)





GO
/****** Object:  StoredProcedure [dbo].[pSelect_General]    Script Date: 12/01/2026 16:57:20 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO





/****** 개체: 저장 프로시저 dbo.pSelect_Avator_Belt    스크립트 날짜: 2003-04-10 오후 9:05:29 ******/
CREATE PROCEDURE [dbo].[pSelect_General]
@serial int
AS
SELECT * 
FROM [dbo].[tbl_general]
WHERE Serial = @serial







GO
/****** Object:  StoredProcedure [dbo].[pSelect_GetCharSerialByNameRace]    Script Date: 12/01/2026 16:57:20 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO






/*
캐릭터의 이름, 종족으로 시리얼을 얻어온다
*/
CREATE PROCEDURE [dbo].[pSelect_GetCharSerialByNameRace]
@charactername varchar(16),	--- 캐릭터 이름
@race1 tinyint,			--- 종족성별 코드
@race2 tinyint			--- 종족성별 코드

AS
SELECT serial FROM [dbo].[tbl_base] WHERE Name = @charactername AND (Race = @race1 OR Race = @race2) AND dck=0






GO
/****** Object:  StoredProcedure [dbo].[pSelect_GoldBoxItem]    Script Date: 12/01/2026 16:57:20 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO



/*  
   작성자 : 이두형
   작성일 : 2008-11-07
   내용    : 황금광 박스 아이템 테이블 읽어오기(프로시저 생성)
*/

/*
   수정자 : 정우철
   작성일 : 2009-01-09
   내용    : 키값과 가장 높을 키값(serial)에 데이터 가져오게 수정
*/

CREATE PROCEDURE [dbo].[pSelect_GoldBoxItem]
AS
select top 1 
serial,
DCK,
StarterBoxMax,
BoxItemK_1,
BoxItemMax_1,
LimItemNum_1,
BoxItemK_2,
BoxItemMax_2,
LimItemNum_2,

K1_0,N1_0,K1_1,N1_1,K1_2,N1_2,K1_3,N1_3,K1_4,N1_4,K1_5,N1_5,K1_6,N1_6,K1_7,N1_7,K1_8,N1_8,K1_9,N1_9,
K1_10,N1_10,K1_11,N1_11,K1_12,N1_12,K1_13,N1_13,K1_14,N1_14,K1_15,N1_15,K1_16,N1_16,K1_17,N1_17,K1_18,N1_18,K1_19,N1_19,
K1_20,N1_20,K1_21,N1_21,K1_22,N1_22,K1_23,N1_23,K1_24,N1_24,K1_25,N1_25,K1_26,N1_26,K1_27,N1_27,K1_28,N1_28,K1_29,N1_29,
K1_30,N1_30,K1_31,N1_31,K1_32,N1_32,K1_33,N1_33,K1_34,N1_34,K1_35,N1_35,K1_36,N1_36,K1_37,N1_37,K1_38,N1_38,K1_39,N1_39,
K1_40,N1_40,K1_41,N1_41,K1_42,N1_42,K1_43,N1_43,K1_44,N1_44,K1_45,N1_45,K1_46,N1_46,K1_47,N1_47,K1_48,N1_48,K1_49,N1_49,
K1_50,N1_50,K1_51,N1_51,K1_52,N1_52,K1_53,N1_53,K1_54,N1_54,K1_55,N1_55,K1_56,N1_56,K1_57,N1_57,K1_58,N1_58,K1_59,N1_59,
K1_60,N1_60,K1_61,N1_61,K1_62,N1_62,K1_63,N1_63,K1_64,N1_64,K1_65,N1_65,K1_66,N1_66,K1_67,N1_67,K1_68,N1_68,K1_69,N1_69,
K1_70,N1_70,K1_71,N1_71,K1_72,N1_72,K1_73,N1_73,K1_74,N1_74,K1_75,N1_75,K1_76,N1_76,K1_77,N1_77,K1_78,N1_78,K1_79,N1_79,
K1_80,N1_80,K1_81,N1_81,K1_82,N1_82,K1_83,N1_83,K1_84,N1_84,K1_85,N1_85,K1_86,N1_86,K1_87,N1_87,K1_88,N1_88,K1_89,N1_89,
K1_90,N1_90,K1_91,N1_91,K1_92,N1_92,K1_93,N1_93,K1_94,N1_94,K1_95,N1_95,K1_96,N1_96,K1_97,N1_97,K1_98,N1_98,K1_99,N1_99,

K2_0,N2_0,K2_1,N2_1,K2_2,N2_2,K2_3,N2_3,K2_4,N2_4,K2_5,N2_5,K2_6,N2_6,K2_7,N2_7,K2_8,N2_8,K2_9,N2_9,
K2_10,N2_10,K2_11,N2_11,K2_12,N2_12,K2_13,N2_13,K2_14,N2_14,K2_15,N2_15,K2_16,N2_16,K2_17,N2_17,K2_18,N2_18,K2_19,N2_19,
K2_20,N2_20,K2_21,N2_21,K2_22,N2_22,K2_23,N2_23,K2_24,N2_24,K2_25,N2_25,K2_26,N2_26,K2_27,N2_27,K2_28,N2_28,K2_29,N2_29,
K2_30,N2_30,K2_31,N2_31,K2_32,N2_32,K2_33,N2_33,K2_34,N2_34,K2_35,N2_35,K2_36,N2_36,K2_37,N2_37,K2_38,N2_38,K2_39,N2_39,
K2_40,N2_40,K2_41,N2_41,K2_42,N2_42,K2_43,N2_43,K2_44,N2_44,K2_45,N2_45,K2_46,N2_46,K2_47,N2_47,K2_48,N2_48,K2_49,N2_49,
K2_50,N2_50,K2_51,N2_51,K2_52,N2_52,K2_53,N2_53,K2_54,N2_54,K2_55,N2_55,K2_56,N2_56,K2_57,N2_57,K2_58,N2_58,K2_59,N2_59,
K2_60,N2_60,K2_61,N2_61,K2_62,N2_62,K2_63,N2_63,K2_64,N2_64,K2_65,N2_65,K2_66,N2_66,K2_67,N2_67,K2_68,N2_68,K2_69,N2_69,
K2_70,N2_70,K2_71,N2_71,K2_72,N2_72,K2_73,N2_73,K2_74,N2_74,K2_75,N2_75,K2_76,N2_76,K2_77,N2_77,K2_78,N2_78,K2_79,N2_79,
K2_80,N2_80,K2_81,N2_81,K2_82,N2_82,K2_83,N2_83,K2_84,N2_84,K2_85,N2_85,K2_86,N2_86,K2_87,N2_87,K2_88,N2_88,K2_89,N2_89,
K2_90,N2_90,K2_91,N2_91,K2_92,N2_92,K2_93,N2_93,K2_94,N2_94,K2_95,N2_95,K2_96,N2_96,K2_97,N2_97,K2_98,N2_98,K2_99,N2_99

from tbl_GoldBoxItem ORDER BY serial DESC


GO
/****** Object:  StoredProcedure [dbo].[pSelect_GuildBattleRank]    Script Date: 12/01/2026 16:57:20 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO






create procedure [dbo].[pSelect_GuildBattleRank]
@serial int
as  
select [Serial] from [dbo].[tbl_GuildBattleRank] where [Serial] = @serial  







GO
/****** Object:  StoredProcedure [dbo].[pSelect_GuildBattleRankList]    Script Date: 12/01/2026 16:57:20 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO






create procedure [dbo].[pSelect_GuildBattleRankList]        
@race tinyint        
as          
select g.serial, g.grade, g.id, r.win, r.draw, r.lose, r.score  
from [dbo].[tbl_GuildBattleRank] as r join [dbo].[tbl_Guild] as g on r.serial = g.Serial        
where g.dck = 0 and g.race = @race and ( r.win > 0 or r.lose > 0 or r.draw > 0 )
order by score desc, win desc, draw desc, lose







GO
/****** Object:  StoredProcedure [dbo].[pSelect_GuildBattleReservedSchedule]    Script Date: 12/01/2026 16:57:20 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO





create procedure [dbo].[pSelect_GuildBattleReservedSchedule]
@startslid int,
@endslid int
as
select G1.Serial, G1.id, G1.Race
, G2.Serial, G2.id, G2.Race
, datepart(hh,  S.StartTime) as StartHour
, datepart(mi,  S.StartTime) as StartMin
, datepart(hh,  dateadd( mi, s.BattleTurm,  S.StartTime ) ) as EndHour
, datepart(mi,  dateadd( mi, s.BattleTurm,  S.StartTime )) as EndMin
from tbl_ReservedGuildBattleInfo as B
join tbl_GuildBattleScheduleInfo as S on B.ID = S.ID
join tbl_Guild as G1 on B.P1GuildSerial = G1.Serial 
join tbl_Guild as G2 on B.P2GuildSerial = G2.Serial
where S.State = 1 and @startslid <= S.SLID and S.SLID <= @endslid
order by S.StartTime







GO
/****** Object:  StoredProcedure [dbo].[pSelect_guildbattleresulloglatest]    Script Date: 12/01/2026 16:57:20 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO




create procedure [dbo].[pSelect_guildbattleresulloglatest]
as 
select top 1 serial from tbl_guildbattleresullog order by serial desc






GO
/****** Object:  StoredProcedure [dbo].[pSelect_GuildBattleTotalRecord]    Script Date: 12/01/2026 16:57:20 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO





create procedure [dbo].[pSelect_GuildBattleTotalRecord]
@serial int
as
select totwin, totlose, totdraw, totcnt from tbl_GuildBattleRank where serial = @serial







GO
/****** Object:  StoredProcedure [dbo].[pSelect_GuildMasterLastConn]    Script Date: 12/01/2026 16:57:20 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO




/*
   작성자 : 김태완
   작성일 : 2007-06-15 (금)
   내용    : 길드마스터 최종 접속시간 조회
*/
CREATE PROCEDURE [dbo].[pSelect_GuildMasterLastConn]
@serial	int,
@limit	int

AS

SELECT	LastConnTime
FROM	[dbo].[tbl_base]
WHERE	Serial = @serial  
AND 	DCK =  0
AND	LastConnTime <= @limit





GO
/****** Object:  StoredProcedure [dbo].[pSelect_GuildSerial]    Script Date: 12/01/2026 16:57:20 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO







CREATE PROCEDURE [dbo].[pSelect_GuildSerial]
@id varchar(17)
AS
SELECT serial FROM [dbo].[tbl_guild] WHERE id = @id and dck=0







GO
/****** Object:  StoredProcedure [dbo].[pSelect_HonorGuild]    Script Date: 12/01/2026 16:57:20 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO








/*
   작성자 : 김태완
   작성일 : 2007-02-21 (수)
   내용    : 명예길드 데이터를 조회한다.
*/
CREATE PROCEDURE [dbo].[pSelect_HonorGuild]
@race tinyint,
@isnew tinyint

AS

SELECT	TOP 5 h.GuildSerial, g.EmblemBack, g.EmblemMark, g.Id, ISNULL((SELECT name FROM [dbo].[tbl_base] WHERE Serial = g.MasterSerial AND DCK=0), '없음'), h.TaxRate
FROM 	[dbo].[tbl_honor_guild] h INNER JOIN [dbo].[tbl_guild] g ON h.GuildSerial = g.Serial
WHERE 	h.Race = @race
AND	h.IsNext = @isnew
AND	h.DCK = 0
AND	g.DCK = 0
ORDER BY TaxRate DESC






GO
/****** Object:  StoredProcedure [dbo].[pSelect_Inven_1]    Script Date: 12/01/2026 16:57:20 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO






/****** 개체: 저장 프로시저 dbo.pSelect_Avator_Inven_1    스크립트 날짜: 2003-04-10 오후 9:05:29 ******/
CREATE PROCEDURE [dbo].[pSelect_Inven_1]
@serial int
AS
SELECT 
K0,D0,U0,
K1,D1,U1,
K2,D2,U2,
K3,D3,U3,
K4,D4,U4,
K5,D5,U5,
K6,D6,U6,
K7,D7,U7,
K8,D8,U8,
K9,D9,U9,
K10,D10,U10,
K11,D11,U11,
K12,D12,U12,
K13,D13,U13,
K14,D14,U14,
K15,D15,U15,
K16,D16,U16,
K17,D17,U17,
K18,D18,U18,
K19,D19,U19

FROM [dbo].[tbl_inven]
WHERE Serial = @serial







GO
/****** Object:  StoredProcedure [dbo].[pSelect_Inven_2]    Script Date: 12/01/2026 16:57:20 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO





/****** 개체: 저장 프로시저 dbo.pSelect_Avator_Inven_2    스크립트 날짜: 2003-04-10 오후 9:05:30 ******/
CREATE PROCEDURE [dbo].[pSelect_Inven_2]
@serial int
AS
SELECT 
K0,D0,U0,
K1,D1,U1,
K2,D2,U2,
K3,D3,U3,
K4,D4,U4,
K5,D5,U5,
K6,D6,U6,
K7,D7,U7,
K8,D8,U8,
K9,D9,U9,
K10,D10,U10,
K11,D11,U11,
K12,D12,U12,
K13,D13,U13,
K14,D14,U14,
K15,D15,U15,
K16,D16,U16,
K17,D17,U17,
K18,D18,U18,
K19,D19,U19,
K20,D20,U20,
K21,D21,U21,
K22,D22,U22,
K23,D23,U23,
K24,D24,U24,
K25,D25,U25,
K26,D26,U26,
K27,D27,U27,
K28,D28,U28,
K29,D29,U29,
K30,D30,U30,
K31,D31,U31,
K32,D32,U32,
K33,D33,U33,
K34,D34,U34,
K35,D35,U35,
K36,D36,U36,
K37,D37,U37,
K38,D38,U38,
K39,D39,U39

FROM [dbo].[tbl_inven]
WHERE Serial = @serial







GO
/****** Object:  StoredProcedure [dbo].[pSelect_Inven_20061115_1]    Script Date: 12/01/2026 16:57:20 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO






/****** 개체: 저장 프로시저 dbo.pSelect_Avator_Inven_1    스크립트 날짜: 2003-04-10 오후 9:05:29 ******/
CREATE PROCEDURE [dbo].[pSelect_Inven_20061115_1]
@serial int
AS
SELECT 
K0,D0,U0,S0,T0,
K1,D1,U1,S1,T1,
K2,D2,U2,S2,T2,
K3,D3,U3,S3,T3,
K4,D4,U4,S4,T4,
K5,D5,U5,S5,T5,
K6,D6,U6,S6,T6,
K7,D7,U7,S7,T7,
K8,D8,U8,S8,T8,
K9,D9,U9,S9,T9,

K10,D10,U10,S10,T10,
K11,D11,U11,S11,T11,
K12,D12,U12,S12,T12,
K13,D13,U13,S13,T13,
K14,D14,U14,S14,T14,
K15,D15,U15,S15,T15,
K16,D16,U16,S16,T16,
K17,D17,U17,S17,T17,
K18,D18,U18,S18,T18,
K19,D19,U19,S19,T19

FROM [dbo].[tbl_inven]
WHERE Serial = @serial






GO
/****** Object:  StoredProcedure [dbo].[pSelect_Inven_20061115_2]    Script Date: 12/01/2026 16:57:20 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO






/****** 개체: 저장 프로시저 dbo.pSelect_Avator_Inven_2    스크립트 날짜: 2003-04-10 오후 9:05:30 ******/
CREATE PROCEDURE [dbo].[pSelect_Inven_20061115_2]
@serial int
AS
SELECT 
K0,D0,U0,S0,T0,
K1,D1,U1,S1,T1,
K2,D2,U2,S2,T2,
K3,D3,U3,S3,T3,
K4,D4,U4,S4,T4,
K5,D5,U5,S5,T5,
K6,D6,U6,S6,T6,
K7,D7,U7,S7,T7,
K8,D8,U8,S8,T8,
K9,D9,U9,S9,T9,
K10,D10,U10,S10,T10,
K11,D11,U11,S11,T11,
K12,D12,U12,S12,T12,
K13,D13,U13,S13,T13,
K14,D14,U14,S14,T14,
K15,D15,U15,S15,T15,
K16,D16,U16,S16,T16,
K17,D17,U17,S17,T17,
K18,D18,U18,S18,T18,
K19,D19,U19,S19,T19,
K20,D20,U20,S20,T20,
K21,D21,U21,S21,T21,
K22,D22,U22,S22,T22,
K23,D23,U23,S23,T23,
K24,D24,U24,S24,T24,
K25,D25,U25,S25,T25,
K26,D26,U26,S26,T26,
K27,D27,U27,S27,T27,
K28,D28,U28,S28,T28,
K29,D29,U29,S29,T29,
K30,D30,U30,S30,T30,
K31,D31,U31,S31,T31,
K32,D32,U32,S32,T32,
K33,D33,U33,S33,T33,
K34,D34,U34,S34,T34,
K35,D35,U35,S35,T35,
K36,D36,U36,S36,T36,
K37,D37,U37,S37,T37,
K38,D38,U38,S38,T38,
K39,D39,U39,S39,T39

FROM [dbo].[tbl_inven]
WHERE Serial = @serial






GO
/****** Object:  StoredProcedure [dbo].[pSelect_Inven_20061115_3]    Script Date: 12/01/2026 16:57:20 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO





/****** 개체: 저장 프로시저 dbo.pSelect_Avator_Inven_3    스크립트 날짜: 2003-04-10 오후 9:05:30 ******/
CREATE PROCEDURE [dbo].[pSelect_Inven_20061115_3]
@serial int
AS
SELECT 
K0,D0,U0,S0,T0,
K1,D1,U1,S1,T1,
K2,D2,U2,S2,T2,
K3,D3,U3,S3,T3,
K4,D4,U4,S4,T4,
K5,D5,U5,S5,T5,
K6,D6,U6,S6,T6,
K7,D7,U7,S7,T7,
K8,D8,U8,S8,T8,
K9,D9,U9,S9,T9,
K10,D10,U10,S10,T10,
K11,D11,U11,S11,T11,
K12,D12,U12,S12,T12,
K13,D13,U13,S13,T13,
K14,D14,U14,S14,T14,
K15,D15,U15,S15,T15,
K16,D16,U16,S16,T16,
K17,D17,U17,S17,T17,
K18,D18,U18,S18,T18,
K19,D19,U19,S19,T19,
K20,D20,U20,S20,T20,
K21,D21,U21,S21,T21,
K22,D22,U22,S22,T22,
K23,D23,U23,S23,T23,
K24,D24,U24,S24,T24,
K25,D25,U25,S25,T25,
K26,D26,U26,S26,T26,
K27,D27,U27,S27,T27,
K28,D28,U28,S28,T28,
K29,D29,U29,S29,T29,
K30,D30,U30,S30,T30,
K31,D31,U31,S31,T31,
K32,D32,U32,S32,T32,
K33,D33,U33,S33,T33,
K34,D34,U34,S34,T34,
K35,D35,U35,S35,T35,
K36,D36,U36,S36,T36,
K37,D37,U37,S37,T37,
K38,D38,U38,S38,T38,
K39,D39,U39,S39,T39,
K40,D40,U40,S40,T40,
K41,D41,U41,S41,T41,
K42,D42,U42,S42,T42,
K43,D43,U43,S43,T43,
K44,D44,U44,S44,T44,
K45,D45,U45,S45,T45,
K46,D46,U46,S46,T46,
K47,D47,U47,S47,T47,
K48,D48,U48,S48,T48,
K49,D49,U49,S49,T49,
K50,D50,U50,S50,T50,
K51,D51,U51,S51,T51,
K52,D52,U52,S52,T52,
K53,D53,U53,S53,T53,
K54,D54,U54,S54,T54,
K55,D55,U55,S55,T55,
K56,D56,U56,S56,T56,
K57,D57,U57,S57,T57,
K58,D58,U58,S58,T58,
K59,D59,U59,S59,T59

FROM [dbo].[tbl_inven]
WHERE Serial = @serial






GO
/****** Object:  StoredProcedure [dbo].[pSelect_Inven_20061115_4]    Script Date: 12/01/2026 16:57:20 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO




/****** 개체: 저장 프로시저 dbo.pSelect_Avator_Inven_4    스크립트 날짜: 2003-04-10 오후 9:05:30 ******/
CREATE PROCEDURE [dbo].[pSelect_Inven_20061115_4]
@serial int
AS
SELECT 
K0,D0,U0,S0,T0,
K1,D1,U1,S1,T1,
K2,D2,U2,S2,T2,
K3,D3,U3,S3,T3,
K4,D4,U4,S4,T4,
K5,D5,U5,S5,T5,
K6,D6,U6,S6,T6,
K7,D7,U7,S7,T7,
K8,D8,U8,S8,T8,
K9,D9,U9,S9,T9,
K10,D10,U10,S10,T10,
K11,D11,U11,S11,T11,
K12,D12,U12,S12,T12,
K13,D13,U13,S13,T13,
K14,D14,U14,S14,T14,
K15,D15,U15,S15,T15,
K16,D16,U16,S16,T16,
K17,D17,U17,S17,T17,
K18,D18,U18,S18,T18,
K19,D19,U19,S19,T19,
K20,D20,U20,S20,T20,
K21,D21,U21,S21,T21,
K22,D22,U22,S22,T22,
K23,D23,U23,S23,T23,
K24,D24,U24,S24,T24,
K25,D25,U25,S25,T25,
K26,D26,U26,S26,T26,
K27,D27,U27,S27,T27,
K28,D28,U28,S28,T28,
K29,D29,U29,S29,T29,
K30,D30,U30,S30,T30,
K31,D31,U31,S31,T31,
K32,D32,U32,S32,T32,
K33,D33,U33,S33,T33,
K34,D34,U34,S34,T34,
K35,D35,U35,S35,T35,
K36,D36,U36,S36,T36,
K37,D37,U37,S37,T37,
K38,D38,U38,S38,T38,
K39,D39,U39,S39,T39,
K40,D40,U40,S40,T40,
K41,D41,U41,S41,T41,
K42,D42,U42,S42,T42,
K43,D43,U43,S43,T43,
K44,D44,U44,S44,T44,
K45,D45,U45,S45,T45,
K46,D46,U46,S46,T46,
K47,D47,U47,S47,T47,
K48,D48,U48,S48,T48,
K49,D49,U49,S49,T49,
K50,D50,U50,S50,T50,
K51,D51,U51,S51,T51,
K52,D52,U52,S52,T52,
K53,D53,U53,S53,T53,
K54,D54,U54,S54,T54,
K55,D55,U55,S55,T55,
K56,D56,U56,S56,T56,
K57,D57,U57,S57,T57,
K58,D58,U58,S58,T58,
K59,D59,U59,S59,T59,
K60,D60,U60,S60,T60,
K61,D61,U61,S61,T61,
K62,D62,U62,S62,T62,
K63,D63,U63,S63,T63,
K64,D64,U64,S64,T64,
K65,D65,U65,S65,T65,
K66,D66,U66,S66,T66,
K67,D67,U67,S67,T67,
K68,D68,U68,S68,T68,
K69,D69,U69,S69,T69,
K70,D70,U70,S70,T70,
K71,D71,U71,S71,T71,
K72,D72,U72,S72,T72,
K73,D73,U73,S73,T73,
K74,D74,U74,S74,T74,
K75,D75,U75,S75,T75,
K76,D76,U76,S76,T76,
K77,D77,U77,S77,T77,
K78,D78,U78,S78,T78,
K79,D79,U79,S79,T79

FROM [dbo].[tbl_inven]
WHERE Serial = @serial






GO
/****** Object:  StoredProcedure [dbo].[pSelect_Inven_20061115_5]    Script Date: 12/01/2026 16:57:20 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO





CREATE PROCEDURE [dbo].[pSelect_Inven_20061115_5]
@serial int
AS
SELECT 
K0,D0,U0,S0,T0,
K1,D1,U1,S1,T1,
K2,D2,U2,S2,T2,
K3,D3,U3,S3,T3,
K4,D4,U4,S4,T4,
K5,D5,U5,S5,T5,
K6,D6,U6,S6,T6,
K7,D7,U7,S7,T7,
K8,D8,U8,S8,T8,
K9,D9,U9,S9,T9,
K10,D10,U10,S10,T10,
K11,D11,U11,S11,T11,
K12,D12,U12,S12,T12,
K13,D13,U13,S13,T13,
K14,D14,U14,S14,T14,
K15,D15,U15,S15,T15,
K16,D16,U16,S16,T16,
K17,D17,U17,S17,T17,
K18,D18,U18,S18,T18,
K19,D19,U19,S19,T19,
K20,D20,U20,S20,T20,
K21,D21,U21,S21,T21,
K22,D22,U22,S22,T22,
K23,D23,U23,S23,T23,
K24,D24,U24,S24,T24,
K25,D25,U25,S25,T25,
K26,D26,U26,S26,T26,
K27,D27,U27,S27,T27,
K28,D28,U28,S28,T28,
K29,D29,U29,S29,T29,
K30,D30,U30,S30,T30,
K31,D31,U31,S31,T31,
K32,D32,U32,S32,T32,
K33,D33,U33,S33,T33,
K34,D34,U34,S34,T34,
K35,D35,U35,S35,T35,
K36,D36,U36,S36,T36,
K37,D37,U37,S37,T37,
K38,D38,U38,S38,T38,
K39,D39,U39,S39,T39,
K40,D40,U40,S40,T40,
K41,D41,U41,S41,T41,
K42,D42,U42,S42,T42,
K43,D43,U43,S43,T43,
K44,D44,U44,S44,T44,
K45,D45,U45,S45,T45,
K46,D46,U46,S46,T46,
K47,D47,U47,S47,T47,
K48,D48,U48,S48,T48,
K49,D49,U49,S49,T49,
K50,D50,U50,S50,T50,
K51,D51,U51,S51,T51,
K52,D52,U52,S52,T52,
K53,D53,U53,S53,T53,
K54,D54,U54,S54,T54,
K55,D55,U55,S55,T55,
K56,D56,U56,S56,T56,
K57,D57,U57,S57,T57,
K58,D58,U58,S58,T58,
K59,D59,U59,S59,T59,
K60,D60,U60,S60,T60,
K61,D61,U61,S61,T61,
K62,D62,U62,S62,T62,
K63,D63,U63,S63,T63,
K64,D64,U64,S64,T64,
K65,D65,U65,S65,T65,
K66,D66,U66,S66,T66,
K67,D67,U67,S67,T67,
K68,D68,U68,S68,T68,
K69,D69,U69,S69,T69,
K70,D70,U70,S70,T70,
K71,D71,U71,S71,T71,
K72,D72,U72,S72,T72,
K73,D73,U73,S73,T73,
K74,D74,U74,S74,T74,
K75,D75,U75,S75,T75,
K76,D76,U76,S76,T76,
K77,D77,U77,S77,T77,
K78,D78,U78,S78,T78,
K79,D79,U79,S79,T79,
K80,D80,U80,S80,T80,
K81,D81,U81,S81,T81,
K82,D82,U82,S82,T82,
K83,D83,U83,S83,T83,
K84,D84,U84,S84,T84,
K85,D85,U85,S85,T85,
K86,D86,U86,S86,T86,
K87,D87,U87,S87,T87,
K88,D88,U88,S88,T88,
K89,D89,U89,S89,T89,
K90,D90,U90,S90,T90,
K91,D91,U91,S91,T91,
K92,D92,U92,S92,T92,
K93,D93,U93,S93,T93,
K94,D94,U94,S94,T94,
K95,D95,U95,S95,T95,
K96,D96,U96,S96,T96,
K97,D97,U97,S97,T97,
K98,D98,U98,S98,T98,
K99,D99,U99,S99,T99
FROM [dbo].[tbl_inven]
WHERE Serial = @serial






GO
/****** Object:  StoredProcedure [dbo].[pSelect_Inven_3]    Script Date: 12/01/2026 16:57:20 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO






/****** 개체: 저장 프로시저 dbo.pSelect_Avator_Inven_3    스크립트 날짜: 2003-04-10 오후 9:05:30 ******/
CREATE PROCEDURE [dbo].[pSelect_Inven_3]
@serial int
AS
SELECT 
K0,D0,U0,
K1,D1,U1,
K2,D2,U2,
K3,D3,U3,
K4,D4,U4,
K5,D5,U5,
K6,D6,U6,
K7,D7,U7,
K8,D8,U8,
K9,D9,U9,
K10,D10,U10,
K11,D11,U11,
K12,D12,U12,
K13,D13,U13,
K14,D14,U14,
K15,D15,U15,
K16,D16,U16,
K17,D17,U17,
K18,D18,U18,
K19,D19,U19,
K20,D20,U20,
K21,D21,U21,
K22,D22,U22,
K23,D23,U23,
K24,D24,U24,
K25,D25,U25,
K26,D26,U26,
K27,D27,U27,
K28,D28,U28,
K29,D29,U29,
K30,D30,U30,
K31,D31,U31,
K32,D32,U32,
K33,D33,U33,
K34,D34,U34,
K35,D35,U35,
K36,D36,U36,
K37,D37,U37,
K38,D38,U38,
K39,D39,U39,
K40,D40,U40,
K41,D41,U41,
K42,D42,U42,
K43,D43,U43,
K44,D44,U44,
K45,D45,U45,
K46,D46,U46,
K47,D47,U47,
K48,D48,U48,
K49,D49,U49,
K50,D50,U50,
K51,D51,U51,
K52,D52,U52,
K53,D53,U53,
K54,D54,U54,
K55,D55,U55,
K56,D56,U56,
K57,D57,U57,
K58,D58,U58,
K59,D59,U59

FROM [dbo].[tbl_inven]
WHERE Serial = @serial







GO
/****** Object:  StoredProcedure [dbo].[pSelect_Inven_4]    Script Date: 12/01/2026 16:57:20 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO





/****** 개체: 저장 프로시저 dbo.pSelect_Avator_Inven_4    스크립트 날짜: 2003-04-10 오후 9:05:30 ******/
CREATE PROCEDURE [dbo].[pSelect_Inven_4]
@serial int
AS
SELECT 
K0,D0,U0,
K1,D1,U1,
K2,D2,U2,
K3,D3,U3,
K4,D4,U4,
K5,D5,U5,
K6,D6,U6,
K7,D7,U7,
K8,D8,U8,
K9,D9,U9,
K10,D10,U10,
K11,D11,U11,
K12,D12,U12,
K13,D13,U13,
K14,D14,U14,
K15,D15,U15,
K16,D16,U16,
K17,D17,U17,
K18,D18,U18,
K19,D19,U19,
K20,D20,U20,
K21,D21,U21,
K22,D22,U22,
K23,D23,U23,
K24,D24,U24,
K25,D25,U25,
K26,D26,U26,
K27,D27,U27,
K28,D28,U28,
K29,D29,U29,
K30,D30,U30,
K31,D31,U31,
K32,D32,U32,
K33,D33,U33,
K34,D34,U34,
K35,D35,U35,
K36,D36,U36,
K37,D37,U37,
K38,D38,U38,
K39,D39,U39,
K40,D40,U40,
K41,D41,U41,
K42,D42,U42,
K43,D43,U43,
K44,D44,U44,
K45,D45,U45,
K46,D46,U46,
K47,D47,U47,
K48,D48,U48,
K49,D49,U49,
K50,D50,U50,
K51,D51,U51,
K52,D52,U52,
K53,D53,U53,
K54,D54,U54,
K55,D55,U55,
K56,D56,U56,
K57,D57,U57,
K58,D58,U58,
K59,D59,U59,
K60,D60,U60,
K61,D61,U61,
K62,D62,U62,
K63,D63,U63,
K64,D64,U64,
K65,D65,U65,
K66,D66,U66,
K67,D67,U67,
K68,D68,U68,
K69,D69,U69,
K70,D70,U70,
K71,D71,U71,
K72,D72,U72,
K73,D73,U73,
K74,D74,U74,
K75,D75,U75,
K76,D76,U76,
K77,D77,U77,
K78,D78,U78,
K79,D79,U79

FROM [dbo].[tbl_inven]
WHERE Serial = @serial







GO
/****** Object:  StoredProcedure [dbo].[pSelect_Inven_5]    Script Date: 12/01/2026 16:57:20 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO






CREATE PROCEDURE [dbo].[pSelect_Inven_5]
@serial int
AS
SELECT 
K0,D0,U0,
K1,D1,U1,
K2,D2,U2,
K3,D3,U3,
K4,D4,U4,
K5,D5,U5,
K6,D6,U6,
K7,D7,U7,
K8,D8,U8,
K9,D9,U9,
K10,D10,U10,
K11,D11,U11,
K12,D12,U12,
K13,D13,U13,
K14,D14,U14,
K15,D15,U15,
K16,D16,U16,
K17,D17,U17,
K18,D18,U18,
K19,D19,U19,
K20,D20,U20,
K21,D21,U21,
K22,D22,U22,
K23,D23,U23,
K24,D24,U24,
K25,D25,U25,
K26,D26,U26,
K27,D27,U27,
K28,D28,U28,
K29,D29,U29,
K30,D30,U30,
K31,D31,U31,
K32,D32,U32,
K33,D33,U33,
K34,D34,U34,
K35,D35,U35,
K36,D36,U36,
K37,D37,U37,
K38,D38,U38,
K39,D39,U39,
K40,D40,U40,
K41,D41,U41,
K42,D42,U42,
K43,D43,U43,
K44,D44,U44,
K45,D45,U45,
K46,D46,U46,
K47,D47,U47,
K48,D48,U48,
K49,D49,U49,
K50,D50,U50,
K51,D51,U51,
K52,D52,U52,
K53,D53,U53,
K54,D54,U54,
K55,D55,U55,
K56,D56,U56,
K57,D57,U57,
K58,D58,U58,
K59,D59,U59,
K60,D60,U60,
K61,D61,U61,
K62,D62,U62,
K63,D63,U63,
K64,D64,U64,
K65,D65,U65,
K66,D66,U66,
K67,D67,U67,
K68,D68,U68,
K69,D69,U69,
K70,D70,U70,
K71,D71,U71,
K72,D72,U72,
K73,D73,U73,
K74,D74,U74,
K75,D75,U75,
K76,D76,U76,
K77,D77,U77,
K78,D78,U78,
K79,D79,U79,
K80,D80,U80,
K81,D81,U81,
K82,D82,U82,
K83,D83,U83,
K84,D84,U84,
K85,D85,U85,
K86,D86,U86,
K87,D87,U87,
K88,D88,U88,
K89,D89,U89,
K90,D90,U90,
K91,D91,U91,
K92,D92,U92,
K93,D93,U93,
K94,D94,U94,
K95,D95,U95,
K96,D96,U96,
K97,D97,U97,
K98,D98,U98,
K99,D99,U99
FROM [dbo].[tbl_inven]
WHERE Serial = @serial







GO
/****** Object:  StoredProcedure [dbo].[pSelect_IsValidChar]    Script Date: 12/01/2026 16:57:20 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO



/*
   작성자 : 김태완
   작성일 : 2007-03-05 (월)
   내용   : 캐릭터가 유효한지 조회
*/
CREATE PROCEDURE [dbo].[pSelect_IsValidChar]
@serial int
AS

SELECT 	Serial
FROM 	[dbo].[tbl_base]
WHERE 	Serial = @serial  
AND 	DCK =  0






GO
/****** Object:  StoredProcedure [dbo].[pSelect_ItemCharge]    Script Date: 12/01/2026 16:57:20 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO







/*
아이템 증여 목록에서 하나만 불러온다
*/
CREATE PROCEDURE [dbo].[pSelect_ItemCharge]
@avatorSerial int
AS
SELECT top 1 nSerial, nItemCode_K, nItemCode_D, nItemCode_U, Type
FROM [dbo].[tbl_ItemCharge]
WHERE nAvatorSerial=@avatorSerial and DCK = 0







GO
/****** Object:  StoredProcedure [dbo].[pSelect_ItemCharge_20070130]    Script Date: 12/01/2026 16:57:20 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO



/*
   작성자 : 이희길
   작성일 : 2007-01-30 (화)
   내용 : 지급받은 아이템의 경우 사용가능한 시간값을 가져와서 체크하도록함.
*/
CREATE PROCEDURE [dbo].[pSelect_ItemCharge_20070130]
@avatorSerial int
AS
SELECT	top 1 nSerial, nItemCode_K, nItemCode_D, nItemCode_U, Type, T
FROM	[dbo].[tbl_ItemCharge]
WHERE	nAvatorSerial=@avatorSerial and DCK = 0







GO
/****** Object:  StoredProcedure [dbo].[pSelect_Last_Economy_History]    Script Date: 12/01/2026 16:57:20 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO








CREATE PROCEDURE [dbo].[pSelect_Last_Economy_History]
@nDate int
AS
SELECT 
top 12 
B_Dalant, C_Dalant, A_Dalant, 
B_Gold, C_Gold, A_Gold,
ManageValue,
B_MineOre1, C_MineOre1, A_MineOre1,
B_MineOre2, C_MineOre2, A_MineOre2,
B_MineOre3, C_MineOre3, A_MineOre3,
B_CutOre1, C_CutOre1, A_CutOre1,
B_CutOre2, C_CutOre2, A_CutOre2,
B_CutOre3, C_CutOre3, A_CutOre3
FROM [dbo].[tbl_economy_history] where Serial < @nDate order by Serial desc







GO
/****** Object:  StoredProcedure [dbo].[pSelect_LoseRaceBattleCount]    Script Date: 12/01/2026 16:57:20 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO




/*
   작성자 : 김태완
   작성일 : 2007-03-02 (금)
   내용    : 최종 접속 종료 후 종족전 패배횟수를 조회한다
*/
CREATE PROCEDURE [dbo].[pSelect_LoseRaceBattleCount]
@race 		tinyint,	-- 종족
@aserial		int	-- 아바타 시리얼

AS

Select	count(*)
From	tbl_racebattle_log
Where	loserace = @race
AND	endtime > (Select LastConnTime From tbl_base Where Serial = @aserial AND DCK=0 AND Lv >= 25)






GO
/****** Object:  StoredProcedure [dbo].[pSelect_NotArrangeCharacter]    Script Date: 12/01/2026 16:57:20 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO







CREATE PROCEDURE [dbo].[pSelect_NotArrangeCharacter]
@serial int
AS
SELECT top 12
Serial, Lv, Race, Dalant, Gold, DeleteName, HomeServer, Class
from [dbo].[tbl_base] where AccountSerial=@serial and arrange=0 and DCK=1






GO
/****** Object:  StoredProcedure [dbo].[pSelect_NpcData]    Script Date: 12/01/2026 16:57:20 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO







CREATE PROCEDURE [dbo].[pSelect_NpcData]
@serial int
AS
SELECT 
Npc0, 
Npc1, 
Npc2, 
Npc3, 
Npc4, 
Npc5 
FROM [dbo].[tbl_NpcData]
WHERE Serial = @serial







GO
/****** Object:  StoredProcedure [dbo].[pSelect_NpcQuest_History]    Script Date: 12/01/2026 16:57:20 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO




/*
   작성자 : 김태완
   작성일 : 2007-05-21 (월)
   내용 : NPC 퀘스트 완료 히스토리 조회
*/
CREATE PROCEDURE [dbo].[pSelect_NpcQuest_History]
@serial int	-- 아바타 시리얼

AS

SELECT	Code1, Level1,
	Code2, Level2, 
	Code3, Level3, 
	Code4, Level4, 
	Code5, Level5, 
	Code6, Level6, 
	Code7, Level7, 
	Code8, Level8, 
	Code9, Level9, 
	Code10, Level10,
	Code11, Level11, 
	Code12, Level12, 
	Code13, Level13, 
	Code14, Level14, 
	Code15, Level15, 
	Code16, Level16, 
	Code17, Level17, 
	Code18, Level18, 
	Code19, Level19, 
	Code20, Level20
FROM 	[dbo].[tbl_npc_quest_history]
WHERE 	Serial = @serial





GO
/****** Object:  StoredProcedure [dbo].[pSelect_NpcQuest_History_20071213]    Script Date: 12/01/2026 16:57:20 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO


/*
   작성자 : 이지혜
   작성일 : 2007-12-13 (목)
   내용 : npc퀘스트 데이타 조회 (완료시간포함)
*/


CREATE  PROCEDURE [dbo].[pSelect_NpcQuest_History_20071213]
@serial int	-- 아바타 시리얼

AS

SELECT	Code1, Level1, Time1,
	Code2, Level2, Time2,
	Code3, Level3, Time3,
	Code4, Level4, Time4,
	Code5, Level5, Time5,
	Code6, Level6, Time6,
	Code7, Level7, Time7,
	Code8, Level8, Time8,
	Code9, Level9, Time9,
	Code10, Level10, Time10,
	Code11, Level11, Time11,
	Code12, Level12, Time12,
	Code13, Level13, Time13,
	Code14, Level14, Time14,
	Code15, Level15, Time15,
	Code16, Level16, Time16,
	Code17, Level17, Time17,
	Code18, Level18, Time18,
	Code19, Level19, Time19,
	Code20, Level20, Time20
FROM 	[dbo].[tbl_npc_quest_history]
WHERE 	Serial = @serial




GO
/****** Object:  StoredProcedure [dbo].[pSelect_NpcQuest_History_20080811]    Script Date: 12/01/2026 16:57:20 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER OFF
GO



/*  
   작성자 : 정우철
   작성일 : 2008-08-11 (월)  
   내용 : pSelect_NpcQuest_History 프로시져 수정
*/  
  
CREATE   PROCEDURE [dbo].[pSelect_NpcQuest_History_20080811]
@serial int -- 아바타 시리얼  
  
AS  
  
SELECT Code1, Level1, Time1, Code2, Level2, Time2, Code3, Level3, Time3,  
 Code4, Level4, Time4, Code5, Level5, Time5, Code6, Level6, Time6,  
 Code7, Level7, Time7, Code8, Level8, Time8, Code9, Level9, Time9,
 Code10, Level10, Time10, Code11, Level11, Time11, Code12, Level12, Time12,
 Code13, Level13, Time13, Code14, Level14, Time14, Code15, Level15, Time15,
 Code16, Level16, Time16, Code17, Level17, Time17, Code18, Level18, Time18,
 Code19, Level19, Time19, Code20, Level20, Time20, Code21, Level21, Time21,
 Code22, Level22, Time22, Code23, Level23, Time23, Code24, Level24, Time24,
 Code25, Level25, Time25, Code26, Level26, Time26, Code27, Level27, Time27,
 Code28, Level28, Time28, Code29, Level29, Time29, Code30, Level30, Time30,
 Code31, Level31, Time31, Code32, Level32, Time32, Code33, Level33, Time33,
 Code34, Level34, Time34, Code35, Level35, Time35, Code36, Level36, Time36,
 Code37, Level37, Time37, Code38, Level38, Time38, Code39, Level39, Time39,
 Code40, Level40, Time40, Code41, Level41, Time41, Code42, Level42, Time42,
 Code43, Level43, Time43, Code44, Level44, Time44, Code45, Level45, Time45,
 Code46, Level46, Time46, Code47, Level47, Time47, Code48, Level48, Time48,
 Code49, Level49, Time49, Code50, Level50, Time50, Code51, Level51, Time51,
 Code52, Level52, Time52, Code53, Level53, Time53, Code54, Level54, Time54,
 Code55, Level55, Time55, Code56, Level56, Time56, Code57, Level57, Time57,
 Code58, Level58, Time58, Code59, Level59, Time59, Code60, Level60, Time60,
 Code61, Level61, Time61, Code62, Level62, Time62, Code63, Level63, Time63,
 Code64, Level64, Time64, Code65, Level65, Time65, Code66, Level66, Time66,
 Code67, Level67, Time67, Code68, Level68, Time68, Code69, Level69, Time69,
 Code70, Level70, Time70
FROM  [dbo].[tbl_npc_quest_history]  
WHERE  Serial = @serial





GO
/****** Object:  StoredProcedure [dbo].[pSelect_Patriarch_Elect]    Script Date: 12/01/2026 16:57:20 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO






/*
   작성자 : 김태완
   작성일 : 2007-02-09 (금)
   내용 : 족장선출 시스템 선출 관리 레코드 조회
*/
CREATE PROCEDURE [dbo].[pSelect_Patriarch_Elect]
@wname varchar(32)

AS

SELECT	TOP 1 serial, proctype, nonvotecnt_0, votecnt_0, nonvotecnt_1, votecnt_1, nonvotecnt_2, votecnt_2
FROM	[dbo].[tbl_patriarch_elect]
WHERE	worldname = @wname
ORDER BY serial DESC







GO
/****** Object:  StoredProcedure [dbo].[pSelect_Patriarch_Elect_20080611]    Script Date: 12/01/2026 16:57:20 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO




/*
   작성자 : 김태완
   작성일 : 2007-02-09 (금)
   내용 : 족장선출 시스템 선출 관리 레코드 조회
   수정 : 2008-06-11 (수) 5등급 투표자 수를 확인하기 위해 기존 프로시져 수정(안우태)
*/
CREATE PROCEDURE [dbo].[pSelect_Patriarch_Elect_20080611]
@wname varchar(32)

AS

SELECT	TOP 1 serial, proctype, nonvotecnt_0, votecnt_0, highgradenum_0, nonvotecnt_1, votecnt_1, highgradenum_1, nonvotecnt_2, votecnt_2, highgradenum_2 
FROM	[dbo].[tbl_patriarch_elect]
WHERE	worldname = @wname
ORDER BY serial DESC




GO
/****** Object:  StoredProcedure [dbo].[pSelect_Patriarch_Elect_20080708]    Script Date: 12/01/2026 16:57:20 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER OFF
GO





/*  
   작성자 : 이두형  
   작성일 : 2008-07-07 (월)  
   내용 : 마지막으로 서버에서 투표정보를 초기화 한 날짜를 확인 하기 위해 프로시져 생성  
*/  
CREATE  PROCEDURE [dbo].[pSelect_Patriarch_Elect_20080708]  
@wname varchar(32)  
  
AS  
  
SELECT TOP 1 serial, proctype, nonvotecnt_0, votecnt_0, highgradenum_0, nonvotecnt_1, votecnt_1, highgradenum_1, nonvotecnt_2, votecnt_2, highgradenum_2, VoteInfoResetTime  
FROM [dbo].[tbl_patriarch_elect]  
WHERE worldname = @wname  
ORDER BY serial DESC  





GO
/****** Object:  StoredProcedure [dbo].[pSelect_PatriarchComm]    Script Date: 12/01/2026 16:57:20 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO







/*
   작성자 : 김태완
   작성일 : 2007-02-08 (목)
   내용 : 캐릭터 시리얼로 무인거래기 족장 입금 수수료 데이터를 조회 한다.
*/
CREATE PROCEDURE [dbo].[pSelect_PatriarchComm]
@aserial int,
@depdate char(8)

AS

SELECT	TOP 7 Dalant, DepositDate 
FROM 	[dbo].[tbl_patriarch_comm] 
WHERE 	AvatorSerial = @aserial 
AND 	DCK=0
AND	DepositDate < @depdate
ORDER BY idx ASC






GO
/****** Object:  StoredProcedure [dbo].[pSelect_PcbangItem]    Script Date: 12/01/2026 16:57:20 ******/
SET ANSI_NULLS OFF
GO
SET QUOTED_IDENTIFIER OFF
GO


/*
   작성자 : 안우태
   작성일 : 2008-02-17
   내용    : pSelect_PcbangItem 프로시져(PC방 혜택 강화)
*/

CREATE PROCEDURE [dbo].[pSelect_PcbangItem]
@serial int
AS
SELECT 
K0,
K1,
K2,
K3,
K4,
K5,
K6,
K7,
K8,
K9,
K10,
K11,
K12,
K13,
K14,
K15,
K16,
K17,
K18,
K19,
K20,
K21,
K22,
K23,
K24,
K25,
K26,
K27,
K28,
K29,
K30,
K31,
K32,
K33,
K34,
K35,
K36,
K37,
K38,
K39,
K40,
K41,
K42,
K43,
K44,
K45,
K46,
K47,
K48,
K49
FROM [dbo].[tbl_pcbangitem]
WHERE Serial = @serial




GO
/****** Object:  StoredProcedure [dbo].[pSelect_ppliinfo]    Script Date: 12/01/2026 16:57:20 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO




create procedure [dbo].[pSelect_ppliinfo]
@serial int
as
select [updatedate], [useup], [limitrate], [originalpoint], [limitpoint], [usepoint] from [dbo].[tbl_pvppointlimitinfo] where [serial] = @serial and dck = 0






GO
/****** Object:  StoredProcedure [dbo].[pSelect_PrimiumPlayTime]    Script Date: 12/01/2026 16:57:20 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO


CREATE PROCEDURE [dbo].[pSelect_PrimiumPlayTime]
@serial int
as
select [LastConnTime], [ContPlayTime], [ForcedClose], [ReceiveCoupon], [EnsureTime]

 from [dbo].[tbl_PrimiumPlayTime] where AccountSerial = @serial






GO
/****** Object:  StoredProcedure [dbo].[pSelect_Punishment]    Script Date: 12/01/2026 16:57:20 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO






/*
캐릭터Serial번호로 캐릭터의 족장제재 정보를 얻어온다
*/
CREATE PROCEDURE [dbo].[pSelect_Punishment]
@ASerial int,	--- 캐릭터 시리얼
@ESerial int	--- 족장단 임기 시리얼
AS
SELECT Type, LTime, ESerial FROM [dbo].[tbl_punishment] WHERE ASerial=@ASerial AND ESerial=@ESerial






GO
/****** Object:  StoredProcedure [dbo].[pSelect_PunishmentCount]    Script Date: 12/01/2026 16:57:20 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO







/*
캐릭터Serial번호로 캐릭터의 족장제재 정보 레코드 카운트를 얻어온다
*/
CREATE PROCEDURE [dbo].[pSelect_PunishmentCount]
@ASerial int,	--- 캐릭터 시리얼
@Type	tinyint	--- 제재 종류
AS
SELECT count(*) FROM [dbo].[tbl_punishment] WHERE ASerial=@ASerial AND Type=@Type






GO
/****** Object:  StoredProcedure [dbo].[pSelect_PvpOrderView]    Script Date: 12/01/2026 16:57:20 ******/
SET ANSI_NULLS OFF
GO
SET QUOTED_IDENTIFIER ON
GO






CREATE   PROCEDURE [dbo].[pSelect_PvpOrderView]
@serial int
as
select [Updatedate], [Death], [Kill], [Todaystacked], [Pvppoint], [PvpTempCash], [PvpCash], 
[KillerSerial0], [KillerSerial1], [KillerSerial2], [KillerSerial3], [KillerSerial4], [KillerSerial5], [KillerSerial6], [KillerSerial7], [KillerSerial8], [KillerSerial9], 
[ContHaveCash], [ContLoseCash], [RaceWarRecvr] from [dbo].[tbl_pvporderview] where serial = @serial




SET QUOTED_IDENTIFIER OFF 




GO
/****** Object:  StoredProcedure [dbo].[pSelect_Quest]    Script Date: 12/01/2026 16:57:20 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO







CREATE PROCEDURE [dbo].[pSelect_Quest]
@serial int
AS
SELECT 
Type0, QuestIndex0, FirstAct0, SecondAct0, ThirdAct0, PassSec0,
Type1, QuestIndex1, FirstAct1, SecondAct1, ThirdAct1, PassSec1,
Type2, QuestIndex2, FirstAct2, SecondAct2, ThirdAct2, PassSec2,
Type3, QuestIndex3, FirstAct3, SecondAct3, ThirdAct3, PassSec3,
Type4, QuestIndex4, FirstAct4, SecondAct4, ThirdAct4, PassSec4,
Type5, QuestIndex5, FirstAct5, SecondAct5, ThirdAct5, PassSec5,
Type6, QuestIndex6, FirstAct6, SecondAct6, ThirdAct6, PassSec6,
Type7, QuestIndex7, FirstAct7, SecondAct7, ThirdAct7, PassSec7,
Type8, QuestIndex8, FirstAct8, SecondAct8, ThirdAct8, PassSec8,
Type9, QuestIndex9, FirstAct9, SecondAct9, ThirdAct9, PassSec9,
Type10, QuestIndex10, FirstAct10, SecondAct10, ThirdAct10, PassSec10,
Type11, QuestIndex11, FirstAct11, SecondAct11, ThirdAct11, PassSec11,
Type12, QuestIndex12, FirstAct12, SecondAct12, ThirdAct12, PassSec12,
Type13, QuestIndex13, FirstAct13, SecondAct13, ThirdAct13, PassSec13,
Type14, QuestIndex14, FirstAct14, SecondAct14, ThirdAct14, PassSec14
FROM [dbo].[tbl_quest]
WHERE Serial = @serial and DCK=0







GO
/****** Object:  StoredProcedure [dbo].[pSelect_Quest_20070611]    Script Date: 12/01/2026 16:57:20 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO




/*
   작성자 : 김태완
   작성일 : 2007-06-11 (월)
   내용 : 퀘스트 데이터 조회 (리스트 확장 15 -> 30)
*/

CREATE PROCEDURE [dbo].[pSelect_Quest_20070611]
@serial int
AS

SELECT 
Type0, QuestIndex0, FirstAct0, SecondAct0, ThirdAct0, PassSec0,
Type1, QuestIndex1, FirstAct1, SecondAct1, ThirdAct1, PassSec1,
Type2, QuestIndex2, FirstAct2, SecondAct2, ThirdAct2, PassSec2,
Type3, QuestIndex3, FirstAct3, SecondAct3, ThirdAct3, PassSec3,
Type4, QuestIndex4, FirstAct4, SecondAct4, ThirdAct4, PassSec4,
Type5, QuestIndex5, FirstAct5, SecondAct5, ThirdAct5, PassSec5,
Type6, QuestIndex6, FirstAct6, SecondAct6, ThirdAct6, PassSec6,
Type7, QuestIndex7, FirstAct7, SecondAct7, ThirdAct7, PassSec7,
Type8, QuestIndex8, FirstAct8, SecondAct8, ThirdAct8, PassSec8,
Type9, QuestIndex9, FirstAct9, SecondAct9, ThirdAct9, PassSec9,
Type10, QuestIndex10, FirstAct10, SecondAct10, ThirdAct10, PassSec10,
Type11, QuestIndex11, FirstAct11, SecondAct11, ThirdAct11, PassSec11,
Type12, QuestIndex12, FirstAct12, SecondAct12, ThirdAct12, PassSec12,
Type13, QuestIndex13, FirstAct13, SecondAct13, ThirdAct13, PassSec13,
Type14, QuestIndex14, FirstAct14, SecondAct14, ThirdAct14, PassSec14,
Type15, QuestIndex15, FirstAct15, SecondAct15, ThirdAct15, PassSec15,
Type16, QuestIndex16, FirstAct16, SecondAct16, ThirdAct16, PassSec16,
Type17, QuestIndex17, FirstAct17, SecondAct17, ThirdAct17, PassSec17,
Type18, QuestIndex18, FirstAct18, SecondAct18, ThirdAct18, PassSec18,
Type19, QuestIndex19, FirstAct19, SecondAct19, ThirdAct19, PassSec19,
Type20, QuestIndex20, FirstAct20, SecondAct20, ThirdAct20, PassSec20,
Type21, QuestIndex21, FirstAct21, SecondAct21, ThirdAct21, PassSec21,
Type22, QuestIndex22, FirstAct22, SecondAct22, ThirdAct22, PassSec22,
Type23, QuestIndex23, FirstAct23, SecondAct23, ThirdAct23, PassSec23,
Type24, QuestIndex24, FirstAct24, SecondAct24, ThirdAct24, PassSec24,
Type25, QuestIndex25, FirstAct25, SecondAct25, ThirdAct25, PassSec25,
Type26, QuestIndex26, FirstAct26, SecondAct26, ThirdAct26, PassSec26,
Type27, QuestIndex27, FirstAct27, SecondAct27, ThirdAct27, PassSec27,
Type28, QuestIndex28, FirstAct28, SecondAct28, ThirdAct28, PassSec28,
Type29, QuestIndex29, FirstAct29, SecondAct29, ThirdAct29, PassSec29
FROM [dbo].[tbl_quest]
WHERE Serial = @serial and DCK=0






GO
/****** Object:  StoredProcedure [dbo].[pSelect_Reged]    Script Date: 12/01/2026 16:57:20 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO






/****** 개체: 저장 프로시저 dbo.pSelect_Reged_Avator    스크립트 날짜: 2003-04-10 오후 9:05:30 ******/
CREATE PROCEDURE [dbo].[pSelect_Reged]
@accountserial int
AS
SELECT Serial, Name, Slot, Race, Class, Lv, Dalant, Gold, BaseShape, Lock,
	EK0, EU0, EK1, EU1, EK2, EU2, EK3, EU3, 
	EK4, EU4, EK5, EU5, EK6, EU6, EK7, EU7,
	LastConnTime
FROM [dbo].[tbl_base]
WHERE AccountSerial = @accountserial  AND DCK =  0







GO
/****** Object:  StoredProcedure [dbo].[pSelect_RemainOre_Log]    Script Date: 12/01/2026 16:57:20 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER OFF
GO
/* 
2008-12-05
작업자 : 임성희
오어 초기화 로그 검색 추가  
*/  
CREATE PROCEDURE [dbo].[pSelect_RemainOre_Log]  
@logDate datetime  
AS  
SELECT LogType,logDate, livenum, remain, totalAmount 
from [dbo].[tbl_RemainOre_Log]
WHERE logDate > @logDate  + '00:00.000' and logDate < @logDate +1

GO
/****** Object:  StoredProcedure [dbo].[pSelect_ReservedGuildBattleInfo]    Script Date: 12/01/2026 16:57:20 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO







create procedure [dbo].[pSelect_ReservedGuildBattleInfo]
@startid int,
@endid int  
as    
select [ID], [P1GuildSerial], [P2GuildSerial], [MapID], [Number] from [dbo].[tbl_ReservedGuildBattleInfo] where @startid <= ID and ID < @endid and P1GuildSerial <> 0 and P2GuildSerial <> 0







GO
/****** Object:  StoredProcedure [dbo].[pSelect_ReservedGuildBattleSchedule]    Script Date: 12/01/2026 16:57:20 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO







create procedure [dbo].[pSelect_ReservedGuildBattleSchedule]
@startid int,    
@endid int    
as    
select [ID], [SLID], [State], [StartTime], [BattleTurm] from [dbo].[tbl_GuildBattleScheduleInfo]
where @startid <= SLID and SLID <= @endid
and State = 1 and StartTime <> 0 and BattleTurm > 0
order by [ID]







GO
/****** Object:  StoredProcedure [dbo].[pSelect_Reset_Accum_PlayTime]    Script Date: 12/01/2026 16:57:20 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER OFF
GO





/****** 개체: 저장 프로시저 현재 날짜와 누적 플레이 시간의 초기화한 날짜의 차이를 가지고 온다. ******/  
CREATE PROCEDURE [dbo].[pSelect_Reset_Accum_PlayTime]  
@serial int  
AS  

declare @day int

SELECT DATEDIFF(minute, ResetAccumPlayTime, getdate())
FROM tbl_Supplement  
WHERE Serial = @serial




GO
/****** Object:  StoredProcedure [dbo].[pSelect_SFDelay]    Script Date: 12/01/2026 16:57:20 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO





/*
   작성자 : 김태완
   작성일 : 2007-04-16 (월)
   내용 : 스킬,포스 딜레이 조회 프로시져
*/
CREATE PROCEDURE [dbo].[pSelect_SFDelay]
@aserial	int

AS

Select effect from tbl_sf_delay where aserial = @aserial






GO
/****** Object:  StoredProcedure [dbo].[pSelect_Start_Npc_Quest_History]    Script Date: 12/01/2026 16:57:20 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO

/****** 개체: 저장 프로시저 NPC 시작 퀘스트에 대한 수행 여부 히스토리를 조회 한다. ******/ 
-- NPC퀘스트 히스토리 셀렉트한다  
CREATE PROCEDURE [dbo].[pSelect_Start_Npc_Quest_History]
@serial int
AS
SELECT  Code, Level, EndTime
FROM [dbo].[tbl_start_npc_quest_history]
WHERE Serial = @serial



GO
/****** Object:  StoredProcedure [dbo].[pSelect_Start_Npc_Quest_History_Count]    Script Date: 12/01/2026 16:57:20 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO


/****** 개체: 저장 프로시저 NPC 시작 퀘스트에 대한 히스토리 개수를 조회 한다. ******/ 
create	procedure [dbo].[pSelect_Start_Npc_Quest_History_Count]
@s_serial  int,
@s_count int output
as
set nocount on

set @s_count = 0

select @s_count = count([serial]) from [dbo].[tbl_start_npc_quest_history] where [serial] = @s_serial

IF (@@ERROR <> 0 )BEGIN	
	RETURN @@ERROR
END

set nocount off



GO
/****** Object:  StoredProcedure [dbo].[pSelect_Supplement]    Script Date: 12/01/2026 16:57:20 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO




CREATE  PROCEDURE [dbo].[pSelect_Supplement]
@aserial int                -- 아바타 시리얼  
AS
select PvpPointLeak, LastAttBuff, BufEndTime
from tbl_supplement
where Serial=@aserial and DCK=0





GO
/****** Object:  StoredProcedure [dbo].[pSelect_Supplement_20080428]    Script Date: 12/01/2026 16:57:20 ******/
SET ANSI_NULLS OFF
GO
SET QUOTED_IDENTIFIER OFF
GO



CREATE   PROCEDURE [dbo].[pSelect_Supplement_20080428]
@aserial int                -- 아바타 시리얼  
AS
select PvpPointLeak, LastAttBuff, BufEndTime, RaceBuffClear 
from tbl_supplement
where Serial=@aserial and DCK=0




GO
/****** Object:  StoredProcedure [dbo].[pSelect_Supplement_Ex_20080707]    Script Date: 12/01/2026 16:57:20 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER OFF
GO



/*  
   작성자 : 이두형  
   작성일 : 2008-07-07 (월)  
   내용  : 저장 프로시저 누적 플레이 시간과 초기화 시간을 초기화 한다.
*/ 
CREATE PROCEDURE [dbo].[pSelect_Supplement_Ex_20080707]  
@serial  int,  
@resetdate  int  
  
AS  
SET NOCOUNT ON  

  
declare @accserial  int  
Select @accserial = AccountSerial From tbl_base Where serial = @serial  
  
  
declare @term  int  
Select @term = ResetAccumPlayTime FROM [dbo].[tbl_Supplement]  WHERE Serial = @serial  
  
if(@term <> @resetdate)  
begin  
 Update [dbo].[tbl_Supplement]  
 Set ResetAccumPlayTime = @resetdate, AccumPlayTime = 0, IsVoted = 0, VoteEnable = 1, ScanerCnt = 0  
 From tbl_Supplement as s Join tbl_base as b ON s.serial = b.serial  
 Where b.accountserial = @accserial AND b.DCK = 0  
end  
  
SELECT AccumPlayTime, ResetAccumPlayTime, IsVoted, VoteEnable, ScanerCnt  
FROM [dbo].[tbl_Supplement]    
WHERE Serial = @serial  
  
SET NOCOUNT OFF  





GO
/****** Object:  StoredProcedure [dbo].[pSelect_Supplement_Ex_20080930]    Script Date: 12/01/2026 16:57:20 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER OFF
GO


/****** 개체: 저장 프로시저 누적 플레이 시간과 초기화 시간을 초기화 한다. ******/ 
CREATE    PROCEDURE [dbo].[pSelect_Supplement_Ex_20080930]
@serial int

AS
SET NOCOUNT ON


SELECT ActionPoint_0, ActionPoint_1,  ActionPoint_2
FROM [dbo].[tbl_Supplement]  
WHERE Serial = @serial

SET NOCOUNT OFF




GO
/****** Object:  StoredProcedure [dbo].[pSelect_TakeItem]    Script Date: 12/01/2026 16:57:20 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO







/*
이미 가져간 아이템 목록을 가져온다
*/
CREATE PROCEDURE [dbo].[pSelect_TakeItem]
@nAvatorSerial int
AS 
SELECT 
top 32 nItemCode_K, nItemCode_D, nItemCode_U, dtTakeDate 
from [dbo].[tbl_ItemCharge]
where nAvatorSerial = @nAvatorSerial and DCK=1 order by dtTakeDate desc







GO
/****** Object:  StoredProcedure [dbo].[pSelect_TakeItem_20061115]    Script Date: 12/01/2026 16:57:20 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO




/*
이미 가져간 아이템 목록을 가져온다
*/
CREATE PROCEDURE [dbo].[pSelect_TakeItem_20061115]
@nAvatorSerial int
AS 
SELECT 
top 32 nItemCode_K, nItemCode_D, nItemCode_U, dtTakeDate, S, T
from [dbo].[tbl_ItemCharge]
where nAvatorSerial = @nAvatorSerial and DCK=1 order by dtTakeDate desc






GO
/****** Object:  StoredProcedure [dbo].[pSelect_Trade]    Script Date: 12/01/2026 16:57:20 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO








CREATE PROCEDURE [dbo].[pSelect_Trade]
@serial int
AS
SELECT 
TrIndex0,TrPrice0,TrPass0,
TrIndex1,TrPrice1,TrPass1,
TrIndex2,TrPrice2,TrPass2,
TrIndex3,TrPrice3,TrPass3,
TrIndex4,TrPrice4,TrPass4,
TrIndex5,TrPrice5,TrPass5,
TrIndex6,TrPrice6,TrPass6,
TrIndex7,TrPrice7,TrPass7,
TrIndex8,TrPrice8,TrPass8,
TrIndex9,TrPrice9,TrPass9
FROM [dbo].[tbl_inven]
WHERE Serial = @serial







GO
/****** Object:  StoredProcedure [dbo].[pSelect_TrunkItemChargeByType]    Script Date: 12/01/2026 16:57:20 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO







/*
창고 증여 목록에서 하나만 불러온다
*/
CREATE PROCEDURE [dbo].[pSelect_TrunkItemChargeByType]
@AccountSerial int,
@type tinyint
as
select top 1 [ID], [TID], [Money], [K], [D], [U], [R] from [dbo].[tbl_AccountTrunkCharge]
where AccountSerial = @AccountSerial and [DCK] = 0 and [TID] = @type







GO
/****** Object:  StoredProcedure [dbo].[pSelect_TrunkItemChargeByType_20070420]    Script Date: 12/01/2026 16:57:20 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO






CREATE PROCEDURE [dbo].[pSelect_TrunkItemChargeByType_20070420]
@AccountSerial int,
@type tinyint
as
select top 1 [ID], [TID], [Money], [K], [D], [U], [R], [T] from [dbo].[tbl_AccountTrunkCharge]
where AccountSerial = @AccountSerial and [DCK] = 0 and [TID] = @type







GO
/****** Object:  StoredProcedure [dbo].[pSelect_TrunkItemChargeByTypeRace]    Script Date: 12/01/2026 16:57:20 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO







/*
창고 증여 목록에서 하나만 불러온다
*/
CREATE PROCEDURE [dbo].[pSelect_TrunkItemChargeByTypeRace]
@AccountSerial int,
@type tinyint,
@race tinyint
as
select top 1 [ID], [TID], [Money], [K], [D], [U], [R] from [dbo].[tbl_AccountTrunkCharge]
where AccountSerial = @AccountSerial and [DCK] = 0 and [TID] = @type and [R] = @race







GO
/****** Object:  StoredProcedure [dbo].[pSelect_TrunkItemChargeByTypeRace_20070420]    Script Date: 12/01/2026 16:57:20 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO



CREATE PROCEDURE [dbo].[pSelect_TrunkItemChargeByTypeRace_20070420]
@AccountSerial int,
@type tinyint,
@race tinyint
as
select top 1 [ID], [TID], [Money], [K], [D], [U], [R], [T] from [dbo].[tbl_AccountTrunkCharge]
where AccountSerial = @AccountSerial and [DCK] = 0 and [TID] = @type and [R] = @race







GO
/****** Object:  StoredProcedure [dbo].[pSelect_TrunkMoney]    Script Date: 12/01/2026 16:57:20 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO







create procedure [dbo].[pSelect_TrunkMoney]
@serial int
as
select Dalant0, Gold0, Dalant1, Gold1, Dalant2, Gold2 from [dbo].[tbl_AccountTrunk] where AccountSerial = @serial







GO
/****** Object:  StoredProcedure [dbo].[pSelect_Unit]    Script Date: 12/01/2026 16:57:20 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO







/*
유닛정보를 가져온다(벨라토 Only)
*/
CREATE PROCEDURE [dbo].[pSelect_Unit]
@serial int
AS
SELECT F_0, F_1, F_2, F_3,
	H_0, H_1, H_2, H_3,
	U_0, U_1, U_2, U_3,
	L_0, L_1, L_2, L_3,
	A_0, A_1, A_2, A_3,
	S_0, S_1, S_2, S_3,
	B_0, B_1, B_2, B_3,
	Gg_0, Gg_1, Gg_2, Gg_3,
	AB_0, AB_1, AB_2, AB_3,
	SB_0, SB_1, SB_2, SB_3,
	KF_0, KF_1, KF_2, KF_3,
	PF_0, PF_1, PF_2, PF_3,
	Cut_0, Cut_1, Cut_2, Cut_3,
	SP0_0, SP0_1, SP0_2, SP0_3,
	SP1_0, SP1_1, SP1_2, SP1_3,
	SP2_0, SP2_1, SP2_2, SP2_3,
	SP3_0, SP3_1, SP3_2, SP3_3,
	SP4_0, SP4_1, SP4_2, SP4_3,
	SP5_0, SP5_1, SP5_2, SP5_3,
	SP6_0, SP6_1, SP6_2, SP6_3,
	SP7_0, SP7_1, SP7_2, SP7_3
FROM [dbo].[tbl_unit] WHERE Serial=@serial







GO
/****** Object:  StoredProcedure [dbo].[pSelect_UnitData]    Script Date: 12/01/2026 16:57:20 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO







create procedure [dbo].[pSelect_UnitData]
@serial int
as
select Data0, Data1 from [dbo].[tbl_unitdata] where serial = @serial







GO
/****** Object:  StoredProcedure [dbo].[pSelect_UserInterface]    Script Date: 12/01/2026 16:57:20 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO









CREATE PROCEDURE [dbo].[pSelect_UserInterface]
@serial int
AS
SELECT 
LB0, LB1, LB2, LB3, LB4, LB5, LB6, LB7, LB8, LB9,
LB10, LB11, LB12, LB13, LB14, LB15, LB16, LB17, LB18, LB19,
LB20, LB21, LB22, LB23, LB24, LB25, LB26, LB27, LB28, LB29,
LB30, LB31, LB32, LB33, LB34, LB35, LB36, LB37, LB38, LB39,
LB40, LB41, LB42, LB43, LB44, LB45, LB46, LB47, LB48, LB49,
DCF0, DCF1, DCF2, DCF3, DCF4, 
HCF0, HCF1, HCF2, HCF3, HCF4,
SkillWin0, SKillWin1, ForceWin0, ForceWin1, 
CharWin0, CharWin1, AnimusWin0, AnimusWin1, 
InvenWin, InvenBag0, InvenBag1, InvenBag2, InvenBag3, InvenBag4, LBLock,
DCF5, DCF6, DCF7,
HCF5, HCF6, HCF7
FROM [dbo].[tbl_userinterface]
WHERE Serial = @serial and DCK=0







GO
/****** Object:  StoredProcedure [dbo].[pSelect_UserInterface0513]    Script Date: 12/01/2026 16:57:20 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO







CREATE PROCEDURE [dbo].[pSelect_UserInterface0513]
@serial int
AS
SELECT 
LB0, LB1, LB2, LB3, LB4, LB5, LB6, LB7, LB8, LB9,
LB10, LB11, LB12, LB13, LB14, LB15, LB16, LB17, LB18, LB19,
LB20, LB21, LB22, LB23, LB24, LB25, LB26, LB27, LB28, LB29,
LB30, LB31, LB32, LB33, LB34, LB35, LB36, LB37, LB38, LB39,
LB40, LB41, LB42, LB43, LB44, LB45, LB46, LB47, LB48, LB49,
DCF0, DCF1, DCF2, DCF3, DCF4, DCF5, DCF6, DCF7,
HCF0, HCF1, HCF2, HCF3, HCF4, HCF5, HCF6, HCF7,
SkillWin0, SKillWin1, ForceWin0, ForceWin1, 
CharWin0, CharWin1, AnimusWin0, AnimusWin1, 
InvenWin, InvenBag0, InvenBag1, InvenBag2, InvenBag3, InvenBag4, LBLock
FROM [dbo].[tbl_userinterface]
WHERE Serial = @serial and DCK=0







GO
/****** Object:  StoredProcedure [dbo].[pSelect_utbuysingleiteminfo]    Script Date: 12/01/2026 16:57:20 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO



CREATE procedure [dbo].[pSelect_utbuysingleiteminfo]  
@type tinyint,  
@serial int  
as  
select si.[inveninx], si.[k], si.[d], si.[u], s.[price], s.[owner]
from [dbo].[tbl_utsellinfo] as s join [dbo].[tbl_utsingleiteminfo] as si  
on s.[type]=@type and s.[serial]=@serial and s.[serial]=si.[serial]






GO
/****** Object:  StoredProcedure [dbo].[pSelect_utbuysingleiteminfo_20061115]    Script Date: 12/01/2026 16:57:20 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO





create procedure [dbo].[pSelect_utbuysingleiteminfo_20061115]
@type tinyint,
@serial int
as
select si.[inveninx], si.[k], si.[d], si.[u], s.[price], s.[owner], si.[s], si.[t]
from [dbo].[tbl_utsellinfo] as s join [dbo].[tbl_utsingleiteminfo] as si
on s.[type]=@type and s.[serial]=@serial and s.[serial]=si.[serial]






GO
/****** Object:  StoredProcedure [dbo].[pSelect_utresultinfostate]    Script Date: 12/01/2026 16:57:20 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO






create procedure [dbo].[pSelect_utresultinfostate]
@type tinyint,
@serial int
as
select [state] from [dbo].[tbl_utresultinfo] where [type]=@type and [serial]=@serial







GO
/****** Object:  StoredProcedure [dbo].[pSelect_utsellinfo]    Script Date: 12/01/2026 16:57:20 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO







create procedure [dbo].[pSelect_utsellinfo]
@type tinyint,
@serial int,
@race tinyint
as
select s.[price], s.[owner], b.[Race], b.[Dalant], g.[GuildSerial], b.[AccountSerial], b.[Account], b.[Name]
 from [dbo].[tbl_utsellinfo] as s join [dbo].[tbl_base] as b
on s.[type]=@type and s.[serial]=@serial and s.[race]=@race and b.[Serial]=s.[owner] and b.[dck]=0
join [dbo].[tbl_general] as g on g.[Serial]=s.[owner]







GO
/****** Object:  StoredProcedure [dbo].[pSelect_utsellinfo_owner]    Script Date: 12/01/2026 16:57:20 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO




/**
 @date 2007-04-27
 @author neon7
 @brief 현재 등록 되어 있는 물품의 소유자 정보를
 가져온다.
 */
create procedure [dbo].[pSelect_utsellinfo_owner]
@type tinyint,
@serial int
as
select owner from [dbo].[tbl_utsellinfo] where [type]=@type and [serial]=@serial






GO
/****** Object:  StoredProcedure [dbo].[pSelect_utsellinfo070426]    Script Date: 12/01/2026 16:57:20 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO




/**
 @date 2007-04-27
 @author neon7
 @brief 등록된 물품 판매시 DB상에 등록된 물품 판매 정보를 가져온다.
 세금이 등록시 소진 되므로 소진된 세금 정보도 추가로 가져온다.
 */
create procedure [dbo].[pSelect_utsellinfo070426]
@type tinyint,  
@serial int,  
@race tinyint  
as  
select s.[price], s.[owner], b.[Race], b.[Dalant], g.[GuildSerial], b.[AccountSerial], b.[Account], b.[Name], r.[tax]
from [dbo].[tbl_utsellinfo] as s join [dbo].[tbl_base] as b  
on s.[type]=@type and s.[serial]=@serial and s.[race]=@race and b.[Serial]=s.[owner] and b.[dck]=0  
join [dbo].[tbl_general] as g on g.[Serial]=s.[owner]
join [dbo].[tbl_utresultinfo] as r on s.[type] = r.[type] and r.[serial] = s.[serial]






GO
/****** Object:  StoredProcedure [dbo].[pSelect_utsellwaititems_salestotals]    Script Date: 12/01/2026 16:57:20 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
/**
 * @date 2008-09-05
 * @author neon7
 * @brief 유저가 로그아웃 상태에서
 * 무인거래기 등록한 물품중에 판매 완료 후 대기중인
 * 아이템의 판매액 총합을 구한다.
 */
create procedure [dbo].[pSelect_utsellwaititems_salestotals]  
@type tinyint,    
@owner int  
as  
select sum( s.[price] - r.[tax] ) from [dbo].[tbl_utresultinfo] as r join [dbo].[tbl_utsellinfo] as s
 on r.[serial] = s.[serial] and s.[owner] = @owner and s.[type] = @type and r.[state] = 4



GO
/****** Object:  StoredProcedure [dbo].[pSelect_WaitItem]    Script Date: 12/01/2026 16:57:20 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO









/*
대기중인 아이템 목록을 가져온다
*/
CREATE PROCEDURE [dbo].[pSelect_WaitItem]
@nAvatorSerial int
AS 
SELECT 
top 32 nItemCode_K, nItemCode_D, nItemCode_U, dtGiveDate 
from [dbo].[tbl_ItemCharge] where nAvatorSerial = @nAvatorSerial and DCK=0 order by dtGiveDate desc







GO
/****** Object:  StoredProcedure [dbo].[pSelect_WaitItem_20061115]    Script Date: 12/01/2026 16:57:20 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO








/*
대기중인 아이템 목록을 가져온다
*/
CREATE PROCEDURE [dbo].[pSelect_WaitItem_20061115]
@nAvatorSerial int
AS 
SELECT top 32 nSerial, nItemCode_K, nItemCode_D, nItemCode_U, dtGiveDate, S, T 
from [dbo].[tbl_ItemCharge] where nAvatorSerial = @nAvatorSerial and DCK=0 order by dtGiveDate desc






GO
/****** Object:  StoredProcedure [dbo].[pSelect_WinRaceBattleCount]    Script Date: 12/01/2026 16:57:20 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO





/*
   작성자 : 김태완
   작성일 : 2007-03-23 (금)
   내용    : 최종 접속 종료 후 종족전 승리 횟수를 조회한다
*/
CREATE PROCEDURE [dbo].[pSelect_WinRaceBattleCount]
@race 		tinyint,	-- 종족
@aserial		int	-- 아바타 시리얼

AS

Select	count(*)
From	tbl_racebattle_log
Where	winrace = @race
AND	endtime > (Select LastConnTime From tbl_base Where Serial = @aserial AND DCK=0 AND Lv >= 25)






GO
/****** Object:  StoredProcedure [dbo].[pUpdate_Accum_PlayTime]    Script Date: 12/01/2026 16:57:20 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER OFF
GO




/****** 개체: 저장 프로시저 누적 플레이 시간의 결과정보를 수정 한다. ******/ 
CREATE PROCEDURE [dbo].[pUpdate_Accum_PlayTime]
@serial int,  
@acctime int

AS
Update tbl_Supplement
Set AccumPlayTime = @acctime
WHERE Serial = @serial




GO
/****** Object:  StoredProcedure [dbo].[pupdate_automine_battery]    Script Date: 12/01/2026 16:57:20 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO






create procedure [dbo].[pupdate_automine_battery]
@ntype tinyint,
@nrace tinyint,
@nguildserial int,
@ngage int
as
update [dbo].[tbl_automine_inven] set battery = @ngage where dck = 0 and collisiontype = @ntype and race = @nrace and guildserial = @nguildserial







GO
/****** Object:  StoredProcedure [dbo].[pupdate_automine_dck]    Script Date: 12/01/2026 16:57:20 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO






CREATE procedure [dbo].[pupdate_automine_dck]
@ntype tinyint,
@nrace tinyint,
@nguildserial int
as
update [dbo].[tbl_automine_inven] set dck = 1, tmLastUpdate = getdate() 
	where dck = 0 and collisiontype = @ntype and race = @nrace






GO
/****** Object:  StoredProcedure [dbo].[pupdate_automine_selore]    Script Date: 12/01/2026 16:57:20 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO






create procedure [dbo].[pupdate_automine_selore]
@ntype tinyint,
@nrace tinyint,
@nguildserial int,
@noreidx tinyint
as
update [dbo].[tbl_automine_inven] set selectore = @noreidx where dck = 0 and collisiontype = @ntype and race = @nrace and guildserial = @nguildserial







GO
/****** Object:  StoredProcedure [dbo].[pupdate_automine_workingstate]    Script Date: 12/01/2026 16:57:20 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO





create procedure [dbo].[pupdate_automine_workingstate]
@ntype tinyint,
@nrace tinyint,
@nguildserial int,
@nwork bit
as
update [dbo].[tbl_automine_inven] set bworking = @nwork where dck = 0 and collisiontype = @ntype and race = @nrace and guildserial = @nguildserial







GO
/****** Object:  StoredProcedure [dbo].[pUpdate_AvatorPostion]    Script Date: 12/01/2026 16:57:20 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO








CREATE PROCEDURE [dbo].[pUpdate_AvatorPostion]
@serial int,
@x float,
@y float,
@z float
AS
UPDATE [dbo].[tbl_general] SET X=@x, Y=@y, Z=@z  WHERE serial = @serial







GO
/****** Object:  StoredProcedure [dbo].[pUpdate_ChangeHonorGuild]    Script Date: 12/01/2026 16:57:20 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO







/*
   작성자 : 김태완
   작성일 : 2007-02-23 (금)
   내용 : 명예길드 수정대기중인 데이터를 현 명예길드로 적용(일자변경에 의해)
*/
CREATE PROCEDURE [dbo].[pUpdate_ChangeHonorGuild]
@race	tinyint
AS

UPDATE	[dbo].[tbl_honor_guild] 
SET 	IsNext = 0, dtUpdate = getdate()
WHERE	Race = @race
AND	IsNext = 1
AND	DCK = 0






GO
/****** Object:  StoredProcedure [dbo].[pUpdate_CharacterReName]    Script Date: 12/01/2026 16:57:20 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO


/*

   작성자 : 안우태

   작성일 : 2007-06-25 (월)

   내용 : 캐쉬 캐릭명 변경 포션 사용시 캐릭명 변경 프로시져

*/

CREATE PROCEDURE [dbo].[pUpdate_CharacterReName]

@charname         nvarchar(16),

@serial                int

AS

 

UPDATE [dbo].[tbl_base] 

SET                    name = @charname

WHERE   Serial = @serial

AND                    DCK = 0






GO
/****** Object:  StoredProcedure [dbo].[pUpdate_ClearGuildBattleInfo]    Script Date: 12/01/2026 16:57:20 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO







create procedure [dbo].[pUpdate_ClearGuildBattleInfo]
@startid int,    
@endid int    
as    
update [dbo].[tbl_ReservedGuildBattleInfo] set [P1GuildSerial]=0, [P2GuildSerial]=0, [MapID]=0, [Number]=0
where @startid <= ID and ID <= @endid







GO
/****** Object:  StoredProcedure [dbo].[pUpdate_ClearGuildBattleRank]    Script Date: 12/01/2026 16:57:20 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO







create procedure [dbo].[pUpdate_ClearGuildBattleRank]
as
update [dbo].[tbl_GuildBattleRank] set win = 0, lose = 0, draw = 0, score = 0







GO
/****** Object:  StoredProcedure [dbo].[pUpdate_ClearGuildBattleScheduleInfo]    Script Date: 12/01/2026 16:57:20 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO







create procedure [dbo].[pUpdate_ClearGuildBattleScheduleInfo]  
@startid int,  
@endid int  
as  
update [dbo].[tbl_GuildBattleScheduleInfo] set State = 0, StartTime = 0, BattleTurm = 0 where @startid <= SLID and SLID <= @endid







GO
/****** Object:  StoredProcedure [dbo].[pUpdate_ClearTrunk]    Script Date: 12/01/2026 16:57:20 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO



-- sp_helptext pInsert_AccountTrunk
-- sp_helptext pSelect_AccountTrunk_0
-- sp_help tbl_accounttrunk


/*
	2006-12-12 창고 정보 초기화 
	CCR.이희길
*/
CREATE PROCEDURE [dbo].[pUpdate_ClearTrunk]
@accountserial int
AS
-- BEGIN
	Update 	tbl_accounttrunk
	set	estslot = 0, trunkpass = 0, hintindex = 0xff, hintanswer = '*',
		k0 = 0xffffffff, d0 = 0, u0 = 0xffffffff, r0 = 0xff, 
		k1 = 0xffffffff, d1 = 0, u1 = 0xffffffff, r1 = 0xff, 
		k2 = 0xffffffff, d2 = 0, u2 = 0xffffffff, r2 = 0xff, 
		k3 = 0xffffffff, d3 = 0, u3 = 0xffffffff, r3 = 0xff, 
		k4 = 0xffffffff, d4 = 0, u4 = 0xffffffff, r4 = 0xff, 
		k5 = 0xffffffff, d5 = 0, u5 = 0xffffffff, r5 = 0xff, 
		k6 = 0xffffffff, d6 = 0, u6 = 0xffffffff, r6 = 0xff, 
		k7 = 0xffffffff, d7 = 0, u7 = 0xffffffff, r7 = 0xff, 
		k8 = 0xffffffff, d8 = 0, u8 = 0xffffffff, r8 = 0xff, 
		k9 = 0xffffffff, d9 = 0, u9 = 0xffffffff, r9 = 0xff, 

		k10 = 0xffffffff, d10 = 0, u10 = 0xffffffff, r10 = 0xff, 
		k11 = 0xffffffff, d11 = 0, u11 = 0xffffffff, r11 = 0xff, 
		k12 = 0xffffffff, d12 = 0, u12 = 0xffffffff, r12 = 0xff, 
		k13 = 0xffffffff, d13 = 0, u13 = 0xffffffff, r13 = 0xff, 
		k14 = 0xffffffff, d14 = 0, u14 = 0xffffffff, r14 = 0xff, 
		k15 = 0xffffffff, d15 = 0, u15 = 0xffffffff, r15 = 0xff, 
		k16 = 0xffffffff, d16 = 0, u16 = 0xffffffff, r16 = 0xff, 
		k17 = 0xffffffff, d17 = 0, u17 = 0xffffffff, r17 = 0xff, 
		k18 = 0xffffffff, d18 = 0, u18 = 0xffffffff, r18 = 0xff, 
		k19 = 0xffffffff, d19 = 0, u19 = 0xffffffff, r19 = 0xff, 

		k20 = 0xffffffff, d20 = 0, u20 = 0xffffffff, r20 = 0xff, 
		k21 = 0xffffffff, d21 = 0, u21 = 0xffffffff, r21 = 0xff, 
		k22 = 0xffffffff, d22 = 0, u22 = 0xffffffff, r22 = 0xff, 
		k23 = 0xffffffff, d23 = 0, u23 = 0xffffffff, r23 = 0xff, 
		k24 = 0xffffffff, d24 = 0, u24 = 0xffffffff, r24 = 0xff, 
		k25 = 0xffffffff, d25 = 0, u25 = 0xffffffff, r25 = 0xff, 
		k26 = 0xffffffff, d26 = 0, u26 = 0xffffffff, r26 = 0xff, 
		k27 = 0xffffffff, d27 = 0, u27 = 0xffffffff, r27 = 0xff, 
		k28 = 0xffffffff, d28 = 0, u28 = 0xffffffff, r28 = 0xff, 
		k29 = 0xffffffff, d29 = 0, u29 = 0xffffffff, r29 = 0xff, 

		k30 = 0xffffffff, d30 = 0, u30 = 0xffffffff, r30 = 0xff, 
		k31 = 0xffffffff, d31 = 0, u31 = 0xffffffff, r31 = 0xff, 
		k32 = 0xffffffff, d32 = 0, u32 = 0xffffffff, r32 = 0xff, 
		k33 = 0xffffffff, d33 = 0, u33 = 0xffffffff, r33 = 0xff, 
		k34 = 0xffffffff, d34 = 0, u34 = 0xffffffff, r34 = 0xff, 
		k35 = 0xffffffff, d35 = 0, u35 = 0xffffffff, r35 = 0xff, 
		k36 = 0xffffffff, d36 = 0, u36 = 0xffffffff, r36 = 0xff, 
		k37 = 0xffffffff, d37 = 0, u37 = 0xffffffff, r37 = 0xff, 
		k38 = 0xffffffff, d38 = 0, u38 = 0xffffffff, r38 = 0xff, 
		k39 = 0xffffffff, d39 = 0, u39 = 0xffffffff, r39 = 0xff, 

		k40 = 0xffffffff, d40 = 0, u40 = 0xffffffff, r40 = 0xff, 
		k41 = 0xffffffff, d41 = 0, u41 = 0xffffffff, r41 = 0xff, 
		k42 = 0xffffffff, d42 = 0, u42 = 0xffffffff, r42 = 0xff, 
		k43 = 0xffffffff, d43 = 0, u43 = 0xffffffff, r43 = 0xff, 
		k44 = 0xffffffff, d44 = 0, u44 = 0xffffffff, r44 = 0xff, 
		k45 = 0xffffffff, d45 = 0, u45 = 0xffffffff, r45 = 0xff, 
		k46 = 0xffffffff, d46 = 0, u46 = 0xffffffff, r46 = 0xff, 
		k47 = 0xffffffff, d47 = 0, u47 = 0xffffffff, r47 = 0xff, 
		k48 = 0xffffffff, d48 = 0, u48 = 0xffffffff, r48 = 0xff, 
		k49 = 0xffffffff, d49 = 0, u49 = 0xffffffff, r49 = 0xff, 

		k50 = 0xffffffff, d50 = 0, u50 = 0xffffffff, r50 = 0xff, 
		k51 = 0xffffffff, d51 = 0, u51 = 0xffffffff, r51 = 0xff, 
		k52 = 0xffffffff, d52 = 0, u52 = 0xffffffff, r52 = 0xff, 
		k53 = 0xffffffff, d53 = 0, u53 = 0xffffffff, r53 = 0xff, 
		k54 = 0xffffffff, d54 = 0, u54 = 0xffffffff, r54 = 0xff, 
		k55 = 0xffffffff, d55 = 0, u55 = 0xffffffff, r55 = 0xff, 
		k56 = 0xffffffff, d56 = 0, u56 = 0xffffffff, r56 = 0xff, 
		k57 = 0xffffffff, d57 = 0, u57 = 0xffffffff, r57 = 0xff, 
		k58 = 0xffffffff, d58 = 0, u58 = 0xffffffff, r58 = 0xff, 
		k59 = 0xffffffff, d59 = 0, u59 = 0xffffffff, r59 = 0xff, 

		k60 = 0xffffffff, d60 = 0, u60 = 0xffffffff, r60 = 0xff, 
		k61 = 0xffffffff, d61 = 0, u61 = 0xffffffff, r61 = 0xff, 
		k62 = 0xffffffff, d62 = 0, u62 = 0xffffffff, r62 = 0xff, 
		k63 = 0xffffffff, d63 = 0, u63 = 0xffffffff, r63 = 0xff, 
		k64 = 0xffffffff, d64 = 0, u64 = 0xffffffff, r64 = 0xff, 
		k65 = 0xffffffff, d65 = 0, u65 = 0xffffffff, r65 = 0xff, 
		k66 = 0xffffffff, d66 = 0, u66 = 0xffffffff, r66 = 0xff, 
		k67 = 0xffffffff, d67 = 0, u67 = 0xffffffff, r67 = 0xff, 
		k68 = 0xffffffff, d68 = 0, u68 = 0xffffffff, r68 = 0xff, 
		k69 = 0xffffffff, d69 = 0, u69 = 0xffffffff, r69 = 0xff, 

		k70 = 0xffffffff, d70 = 0, u70 = 0xffffffff, r70 = 0xff, 
		k71 = 0xffffffff, d71 = 0, u71 = 0xffffffff, r71 = 0xff, 
		k72 = 0xffffffff, d72 = 0, u72 = 0xffffffff, r72 = 0xff, 
		k73 = 0xffffffff, d73 = 0, u73 = 0xffffffff, r73 = 0xff, 
		k74 = 0xffffffff, d74 = 0, u74 = 0xffffffff, r74 = 0xff, 
		k75 = 0xffffffff, d75 = 0, u75 = 0xffffffff, r75 = 0xff, 
		k76 = 0xffffffff, d76 = 0, u76 = 0xffffffff, r76 = 0xff, 
		k77 = 0xffffffff, d77 = 0, u77 = 0xffffffff, r77 = 0xff, 
		k78 = 0xffffffff, d78 = 0, u78 = 0xffffffff, r78 = 0xff, 
		k79 = 0xffffffff, d79 = 0, u79 = 0xffffffff, r79 = 0xff, 

		k80 = 0xffffffff, d80 = 0, u80 = 0xffffffff, r80 = 0xff, 
		k81 = 0xffffffff, d81 = 0, u81 = 0xffffffff, r81 = 0xff, 
		k82 = 0xffffffff, d82 = 0, u82 = 0xffffffff, r82 = 0xff, 
		k83 = 0xffffffff, d83 = 0, u83 = 0xffffffff, r83 = 0xff, 
		k84 = 0xffffffff, d84 = 0, u84 = 0xffffffff, r84 = 0xff, 
		k85 = 0xffffffff, d85 = 0, u85 = 0xffffffff, r85 = 0xff, 
		k86 = 0xffffffff, d86 = 0, u86 = 0xffffffff, r86 = 0xff, 
		k87 = 0xffffffff, d87 = 0, u87 = 0xffffffff, r87 = 0xff, 
		k88 = 0xffffffff, d88 = 0, u88 = 0xffffffff, r88 = 0xff, 
		k89 = 0xffffffff, d89 = 0, u89 = 0xffffffff, r89 = 0xff, 

		k90 = 0xffffffff, d90 = 0, u90 = 0xffffffff, r90 = 0xff, 
		k91 = 0xffffffff, d91 = 0, u91 = 0xffffffff, r91 = 0xff, 
		k92 = 0xffffffff, d92 = 0, u92 = 0xffffffff, r92 = 0xff, 
		k93 = 0xffffffff, d93 = 0, u93 = 0xffffffff, r93 = 0xff, 
		k94 = 0xffffffff, d94 = 0, u94 = 0xffffffff, r94 = 0xff, 
		k95 = 0xffffffff, d95 = 0, u95 = 0xffffffff, r95 = 0xff, 
		k96 = 0xffffffff, d96 = 0, u96 = 0xffffffff, r96 = 0xff, 
		k97 = 0xffffffff, d97 = 0, u97 = 0xffffffff, r97 = 0xff, 
		k98 = 0xffffffff, d98 = 0, u98 = 0xffffffff, r98 = 0xff, 
		k99 = 0xffffffff, d99 = 0, u99 = 0xffffffff, r99 = 0xff

	Where	accountserial = @accountserial
-- END






GO
/****** Object:  StoredProcedure [dbo].[pUpdate_CombineEx_Result_Pop]    Script Date: 12/01/2026 16:57:20 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO







-- 조합결과 업데이트
CREATE PROCEDURE [dbo].[pUpdate_CombineEx_Result_Pop]

@_Serial int

AS
update [dbo].[tbl_itemcombine_ex_result]
set 

IsResult = 0,
CheckKey = 0xffffffff,
DlgType = 0xff,
Dalant = 0,
ItemListCount = 0,
ItemSelectCount = 0

where Serial=@_Serial







GO
/****** Object:  StoredProcedure [dbo].[pUpdate_CristalBattleDate]    Script Date: 12/01/2026 16:57:20 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO



create procedure [dbo].[pUpdate_CristalBattleDate]
@serial int,
@csTime tinyint
as

declare @ics int
set @ics = convert(int, convert(varchar, getdate(), 112) + convert(varchar, @csTime) )

update [dbo].[tbl_supplement] set cristalbattledate = @ics where serial = @serial






GO
/****** Object:  StoredProcedure [dbo].[pUpdate_DeleteGuild]    Script Date: 12/01/2026 16:57:20 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO



CREATE procedure [dbo].[pUpdate_DeleteGuild]
@serial int
as  
update [dbo].[tbl_guild] set [dck]=1, [deleteid]=[id], [id]='*'+convert(varchar, [serial]) where [serial]=@serial






GO
/****** Object:  StoredProcedure [dbo].[pUpdate_DeleteName_Step1]    Script Date: 12/01/2026 16:57:20 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO









CREATE PROCEDURE [dbo].[pUpdate_DeleteName_Step1]
@serial int
AS
UPDATE [dbo].[tbl_base] SET deletename=name  WHERE serial = @serial







GO
/****** Object:  StoredProcedure [dbo].[pUpdate_DeleteName_Step2]    Script Date: 12/01/2026 16:57:20 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO







CREATE PROCEDURE [dbo].[pUpdate_DeleteName_Step2]
@serial int
AS
UPDATE [dbo].[tbl_base] SET name='*'+cast(serial as varchar)  WHERE serial = @serial







GO
/****** Object:  StoredProcedure [dbo].[pUpdate_DrawGuildBattleRank]    Script Date: 12/01/2026 16:57:20 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO






create procedure [dbo].[pUpdate_DrawGuildBattleRank]  
@serial int,  
@score int
as  
update [dbo].[tbl_GuildBattleRank] set draw=draw+1, score=score+@score,  totdraw=totdraw+1, totcnt=totcnt+1 where serial = @serial  







GO
/****** Object:  StoredProcedure [dbo].[pUpdate_Economy_History]    Script Date: 12/01/2026 16:57:20 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO







CREATE PROCEDURE [dbo].[pUpdate_Economy_History]
@nDate int,
@fBDalant float,
@fBGold float,
@fCDalant float,
@fCGold float,
@fADalant float,
@fAGold float,
@nManageValue int,

@fBMineOre1 float,
@fCMineOre1 float,
@fAMineOre1 float,
@fBMineOre2 float,
@fCMineOre2 float,
@fAMineOre2 float,
@fBMineOre3 float,
@fCMineOre3 float,
@fAMineOre3 float,
@fBCutOre1 float,
@fCCutOre1 float,
@fACutOre1 float,
@fBCutOre2 float,
@fCCutOre2 float,
@fACutOre2 float,
@fBCutOre3 float,
@fCCutOre3 float,
@fACutOre3 float

AS
update [dbo].[tbl_economy_history]
set 
B_Dalant = @fBDalant, B_Gold = @fBGold, 
C_Dalant = @fCDalant, C_Gold = @fCGold, 
A_Dalant = @fADalant, A_Gold = @fAGold, 
ManageValue = @nManageValue,

B_MineOre1 = @fBMineOre1, C_MineOre1 = @fCMineOre1, A_MineOre1 = @fAMineOre1,
B_MineOre2 = @fBMineOre2, C_MineOre2 = @fCMineOre2, A_MineOre2 = @fAMineOre2,
B_MineOre3 = @fBMineOre3, C_MineOre3 = @fCMineOre3, A_MineOre3 = @fAMineOre3,
B_CutOre1 = @fBCutOre1, C_CutOre1 = @fCCutOre1, A_CutOre1 = @fACutOre1,
B_CutOre2 = @fBCutOre2, C_CutOre2 = @fCCutOre2, A_CutOre2 = @fACutOre2,
B_CutOre3 = @fBCutOre3, C_CutOre3 = @fCCutOre3, A_CutOre3 = @fACutOre3

where Serial=@nDate







GO
/****** Object:  StoredProcedure [dbo].[pUpdate_Get_Scaner_Cnt_20080701]    Script Date: 12/01/2026 16:57:20 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER OFF
GO




/****** 개체: 저장 프로시저 종족전에서의 스캐너 획득 수의 결과정보를 수정 한다. ******/   
CREATE  PROCEDURE [dbo].[pUpdate_Get_Scaner_Cnt_20080701]  
@serial int,    
@csTime int,  
@cnt int  
  
AS  
  
declare @ics int  
set @ics = convert(int, convert(varchar, getdate(), 112) + convert(varchar, @csTime) + convert(varchar, @cnt) )  
  
Update [dbo].[tbl_Supplement]  
Set ScanerCnt = @ics  
WHERE Serial = @serial  




GO
/****** Object:  StoredProcedure [dbo].[pUpdate_guildbattleresultlog]    Script Date: 12/01/2026 16:57:20 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO




create procedure [dbo].[pUpdate_guildbattleresultlog]
@logserial int,
@redserial int,
@blueserial int
as
update [dbo].[tbl_guildbattleresullog]
set
[redweeklywin] = r.[win],
[redweeklylose] = r.[lose],
[redweeklydraw] = r.[draw],
[redweeklywinscore]= r.[score],
[redtotwin] = r.[totwin],
[redtotlose] = r.[totlose],
[redtotdraw] = r.[totdraw],
[redtotbattlecnt] = r.[totcnt],

[blueweeklywin] = b.[win],
[blueweeklylose] = b.[lose],
[blueweeklydraw] = b.[draw],
[blueweeklywinscore] = b.[score],
[bluetotwin] = b.[totwin],
[bluetotlose] = b.[totlose],
[bluetotdraw] = b.[totdraw],
[bluetotbattlecnt] = b.[totcnt],

[redguildbattlepvppoint]= rp.[guildbattlepvppoint],
[blueguildbattlepvppoint] = bp.[guildbattlepvppoint]
from
( select [win], [lose], [draw], [score], [totwin], [totlose], [totdraw], [totcnt]  from [dbo].[tbl_GuildBattleRank] where [serial] = @redserial ) as r
, ( select [win], [lose], [draw], [score], [totwin], [totlose], [totdraw], [totcnt]  from [dbo].[tbl_GuildBattleRank] where [serial] = @blueserial ) as b
, ( select [guildbattlepvppoint] from tbl_WeeklyGuildPVPPointSum where [serial] = @redserial ) as rp
, ( select [guildbattlepvppoint] from tbl_WeeklyGuildPVPPointSum where [serial] = @blueserial ) as bp
where [dbo].[tbl_guildbattleresullog].[serial] = @logserial






GO
/****** Object:  StoredProcedure [dbo].[pUpdate_GuildMaster]    Script Date: 12/01/2026 16:57:20 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO







-- 길드 마스터를 세팅한다.  
create procedure [dbo].[pUpdate_GuildMaster]
@guildserial int,  
@masterserial int,  
@beforgrade int  
as
update [dbo].[tbl_Guild] set MasterSerial = @masterserial,  MasterBeforeGrade = @beforgrade
where Serial = @guildserial







GO
/****** Object:  StoredProcedure [dbo].[pUpdate_HonorGuild]    Script Date: 12/01/2026 16:57:20 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO







/*
   작성자 : 김태완
   작성일 : 2007-02-22 (목)
   내용 : 명예길드 기존 데이터 Disable 처리
*/
CREATE PROCEDURE [dbo].[pUpdate_HonorGuild]
@race	tinyint,
@isnext	tinyint
AS

UPDATE	[dbo].[tbl_honor_guild] 
SET 	DCK = 1, dtDisable = getdate()
WHERE	Race = @race
AND	IsNext = @isnext
AND	DCK = 0






GO
/****** Object:  StoredProcedure [dbo].[pUpdate_Level]    Script Date: 12/01/2026 16:57:20 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO








CREATE PROCEDURE [dbo].[pUpdate_Level]
@serial int,
@Lv int
AS
UPDATE [dbo].[tbl_base] SET Lv=@Lv  WHERE serial = @serial







GO
/****** Object:  StoredProcedure [dbo].[pUpdate_LoseGuildBattleRank]    Script Date: 12/01/2026 16:57:20 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO






create procedure [dbo].[pUpdate_LoseGuildBattleRank]  
@serial int,
@score int
as  
update [dbo].[tbl_GuildBattleRank] set lose=lose+1, score=score+@score, totlose=totlose+1, totcnt=totcnt+1 where serial = @serial  







GO
/****** Object:  StoredProcedure [dbo].[pUpdate_Patriarch_ClassType]    Script Date: 12/01/2026 16:57:20 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO






/*
   작성자 : 김태완
   작성일 : 2007-02-09 (금)
   내용 : 족장선출 시스템 선출된 족장진 정보 업데이트
*/
CREATE PROCEDURE [dbo].[pUpdate_Patriarch_ClassType]
@class	tinyint,
@eserial	int,
@aserial	int

AS

UPDATE 	[dbo].[tbl_patriarch_candidate]
SET 	ClassType = @class, WinCnt = 1
WHERE	eSerial = @eserial 
AND 	ASerial = @aserial







GO
/****** Object:  StoredProcedure [dbo].[pUpdate_Patriarch_Elect]    Script Date: 12/01/2026 16:57:20 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO






/*
   작성자 : 김태완
   작성일 : 2007-02-09 (금)
   내용 : 족장선출 시스템 선출 레코드 업데이트
*/
CREATE PROCEDURE [dbo].[pUpdate_Patriarch_Elect]
@ptype	tinyint,
@nvote0	int,
@tvote0	int,
@nvote1	int,
@tvote1	int,
@nvote2	int,
@tvote2	int,
@eserial int

AS

UPDATE	[dbo].[tbl_patriarch_elect]
SET	ProcType=@ptype, NonVoteCnt_0=@nvote0, VoteCnt_0=@tvote0, NonVoteCnt_1=@nvote1, VoteCnt_1=@tvote1, 
	NonVoteCnt_2=@nvote2, VoteCnt_2=@tvote2, tmUpdate=getdate()
WHERE	Serial=@eserial







GO
/****** Object:  StoredProcedure [dbo].[pUpdate_Patriarch_Elect_20080611]    Script Date: 12/01/2026 16:57:20 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO

/*
   작성자 : 김태완
   작성일 : 2007-02-09 (금)
   내용 : 족장선출 시스템 선출 레코드 입력
   수정 : 2008-06-11 (수) 5등급 투표자 수를 확인하기 위해 기존 프로시져 수정(안우태)
*/
CREATE PROCEDURE [dbo].[pUpdate_Patriarch_Elect_20080611]
@ptype	tinyint,
@nvote0	int,
@tvote0	int,
@highgrade0	int,
@nvote1	int,
@tvote1	int,
@highgrade1	int,
@nvote2	int,
@tvote2	int,
@highgrade2	int,
@eserial int

AS

UPDATE	[dbo].[tbl_patriarch_elect]
SET	ProcType=@ptype, NonVoteCnt_0=@nvote0, VoteCnt_0=@tvote0, HighGradeNum_0=@highgrade0, 
	NonVoteCnt_1=@nvote1, VoteCnt_1=@tvote1,  HighGradeNum_1=@highgrade1, 
	NonVoteCnt_2=@nvote2, VoteCnt_2=@tvote2,  HighGradeNum_2=@highgrade2, tmUpdate=getdate()
WHERE	Serial=@eserial




GO
/****** Object:  StoredProcedure [dbo].[pUpdate_Patriarch_Elect_ResetTime]    Script Date: 12/01/2026 16:57:20 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER OFF
GO





/*  
   작성자 : 이두형  
   작성일 : 2008-07-07 (월)  
   내용  : 마지막으로 서버에서 투표정보를 초기화 한 날짜를 수정 하기 위한 프로시져 생성  
*/  
  
CREATE      PROCEDURE [dbo].[pUpdate_Patriarch_Elect_ResetTime]  
@reset  int,  
@ptype  tinyint,  
@eserial   int  
  
  
AS  
  
UPDATE [dbo].[tbl_patriarch_elect]  
SET ProcType=@ptype, VoteInfoResetTime = @reset  
WHERE Serial=@eserial  






GO
/****** Object:  StoredProcedure [dbo].[pUpdate_Patriarch_PayRefund]    Script Date: 12/01/2026 16:57:20 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO






/*
   작성자 : 김태완
   작성일 : 2007-02-09 (금)
   내용 : 족장선출 시스템 후보자 환급 처리 업데이트
*/
CREATE PROCEDURE [dbo].[pUpdate_Patriarch_PayRefund]
@race	tinyint,
@aserial	int

AS

UPDATE	[dbo].[tbl_patriarch_candidate]
SET	Refund = 0
WHERE	Race = @race
AND	ASerial = @aserial
AND	Refund <> 0







GO
/****** Object:  StoredProcedure [dbo].[pUpdate_Patriarch_Refund]    Script Date: 12/01/2026 16:57:20 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO






/*
   작성자 : 김태완
   작성일 : 2007-02-09 (금)
   내용 : 족장선출 시스템 후보자 환급 정보 업데이트
*/
CREATE PROCEDURE [dbo].[pUpdate_Patriarch_Refund]
@refund	tinyint,
@eserial	int,
@aserial	int,
@race	tinyint

AS

UPDATE	tbl_patriarch_candidate
SET	Refund = @refund
WHERE	ESerial = @eserial 
AND	ASerial = @aserial
AND	Race = @race







GO
/****** Object:  StoredProcedure [dbo].[pUpdate_Patriarch_Score]    Script Date: 12/01/2026 16:57:20 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO







/*
   작성자 : 김태완
   작성일 : 2007-02-12 (월)
   내용 : 족장선출 시스템 후보자 득표수 업데이트
*/
CREATE PROCEDURE [dbo].[pUpdate_Patriarch_Score]
@score 	int,
@eserial	int,
@aserial	int,
@race	tinyint

AS

UPDATE 	tbl_patriarch_candidate
SET	Score = @score, dtUpScore = getdate()
WHERE	ESerial = @eserial
AND	ASerial = @aserial
AND	Race = @race







GO
/****** Object:  StoredProcedure [dbo].[pUpdate_Patriarch_Status]    Script Date: 12/01/2026 16:57:20 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO







/*
   작성자 : 김태완
   작성일 : 2007-02-09 (금)
   내용 : 족장선출 시스템 선출된 족장진 정보 업데이트
*/
CREATE PROCEDURE [dbo].[pUpdate_Patriarch_Status]
@status tinyint,
@eserial int,
@aserial int,
@race	tinyint

AS

UPDATE	[dbo].[tbl_patriarch_candidate]
SET	state = @status
WHERE	eSerial = @eserial
AND	ASerial = @aserial
AND	Race = @race







GO
/****** Object:  StoredProcedure [dbo].[pUpdate_PatriarchComm]    Script Date: 12/01/2026 16:57:20 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO







/*
   작성자 : 김태완
   작성일 : 2007-02-08 (목)
   내용 : 무인거래기 족장 입금 수수료 데이터를 업데이트 한다.(일일 수수료 누적시 사용)
*/
CREATE PROCEDURE [dbo].[pUpdate_PatriarchComm]
@aserial int,
@dalant int,
@depdate char(8)
AS

UPDATE	[dbo].[tbl_patriarch_comm] 
SET 	Dalant=Dalant + @dalant
WHERE	AvatorSerial = @aserial
AND	DepositDate = @depdate
AND	DCK = 0






GO
/****** Object:  StoredProcedure [dbo].[pUpdate_Punishment]    Script Date: 12/01/2026 16:57:20 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO






/*
캐릭터의 족장 제재 정보를 업데이트 한다.
*/
CREATE PROCEDURE [dbo].[pUpdate_Punishment] 
@aserial int,	--- 캐릭터 시리얼
@eserial int,	--- 족장단 임기 시리얼
@type tinyint,	--- 제재 종류
@value int	--- 제재 시간(분)

AS

UPDATE tbl_punishment SET ESerial = @eserial, LTime=@value WHERE ASerial = @aserial







GO
/****** Object:  StoredProcedure [dbo].[pUpdate_PvpPointInfo]    Script Date: 12/01/2026 16:57:20 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO







CREATE PROCEDURE [dbo].[pUpdate_PvpPointInfo]
@serial int,
@Class0 int,
@Class1 int,
@Class2 int,
@PvpPoint int
AS

UPDATE [dbo].[tbl_general] SET 
Class0=@Class0, 
Class1=@Class1, 
Class2=@Class2, 
PvpPoint=@PvpPoint
WHERE serial = @serial







GO
/****** Object:  StoredProcedure [dbo].[pUpdate_ReservedGuildBattleInfo]    Script Date: 12/01/2026 16:57:20 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO






create procedure [dbo].[pUpdate_ReservedGuildBattleInfo]  
@id int,
@p1serial int,
@p2serial int,
@mapid int,
@number tinyint
as  
update [dbo].[tbl_ReservedGuildBattleInfo] set [P1GuildSerial]=@p1serial, [P2GuildSerial]=@p2serial, [MapID]=@mapid, [Number] = @number  
where ID = @id







GO
/****** Object:  StoredProcedure [dbo].[pUpdate_Reset_Race_Vote_Inform_20080708]    Script Date: 12/01/2026 16:57:20 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER OFF
GO





/****** 개체: 저장 프로시저 하나의 계정에 대한 누적 투표 가능 여부를 수정 한다.******/   
CREATE  PROCEDURE [dbo].[pUpdate_Reset_Race_Vote_Inform_20080708]  
@accserial int,   
@reset  int  
  
  
AS  
  
Update [dbo].[tbl_Supplement]   
Set VoteEnable = 1, IsVoted = 0, ScanerCnt = 0, AccumPlayTime = 0, ResetAccumPlayTime = @reset  
From tbl_Supplement as s Join tbl_base as b ON s.serial = b.serial  
Where b.accountserial = @accserial AND b.DCK = 0  






GO
/****** Object:  StoredProcedure [dbo].[pUpdate_ScheduleInfo]    Script Date: 12/01/2026 16:57:20 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO






create procedure [dbo].[pUpdate_ScheduleInfo]    
@id int,    
@state tinyint,    
@starttime datetime,    
@battleturm smallint  
as    
update [dbo].[tbl_GuildBattleScheduleInfo] set State = @state, StartTime = @starttime, BattleTurm = @battleturm where id = @id    







GO
/****** Object:  StoredProcedure [dbo].[pUpdate_sellInfo]    Script Date: 12/01/2026 16:57:20 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO






create procedure [dbo].[pUpdate_sellInfo]
@type tinyint,
@serial int,
@race tinyint,
@regdate datetime,
@owner int,
@price float,
@sellturm tinyint
as
update [dbo].[tbl_utsellinfo] set [race]=@race, [regdate]=@regdate, [owner] = @owner, [price] = @price, [sellturm] = @sellturm where [type] = @type and [serial] = @serial







GO
/****** Object:  StoredProcedure [dbo].[pUpdate_SetActive]    Script Date: 12/01/2026 16:57:20 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO






CREATE PROCEDURE [dbo].[pUpdate_SetActive]
@serial int,
@newname varchar(17),
@newslot int
AS
update [dbo].[tbl_base] set dck=0, arrange=1, name=@newname, slot = @newslot
where serial = @serial







GO
/****** Object:  StoredProcedure [dbo].[pUpdate_SFDelay]    Script Date: 12/01/2026 16:57:20 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO





/*
   작성자 : 김태완
   작성일 : 2007-04-16 (월)
   내용 : 스킬,포스 딜레이 업데이트 프로시져
*/
CREATE PROCEDURE [dbo].[pUpdate_SFDelay]
@aserial	int,
@effect	binary(130)

AS

Update tbl_sf_delay Set effect = @effect Where aserial = @aserial






GO
/****** Object:  StoredProcedure [dbo].[pUpdate_Start_Npc_Quest_History_Type1]    Script Date: 12/01/2026 16:57:20 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO

/****** 개체: 저장 프로시저 NPC 시작 퀘스트에 대한 수행 여부 히스토리를 수정 한다. ******/ 
CREATE PROCEDURE [dbo].[pUpdate_Start_Npc_Quest_History_Type1]
@serial int,
@code varchar(64),
@level  tinyint,
@startdate datetime,
@endtime bigint


AS 
Update [dbo].[tbl_start_npc_quest_history]
Set Level = @level, startdate = @startdate, EndTime = @endtime
Where Serial =  @serial AND Code = @code



GO
/****** Object:  StoredProcedure [dbo].[pUpdate_Supplement_Ex_20080609]    Script Date: 12/01/2026 16:57:20 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER OFF
GO



/****** 개체: 저장 프로시저 누적 플레이 시간과 초기화 시간을 초기화 한다. ******/ 
CREATE  PROCEDURE [dbo].[pUpdate_Supplement_Ex_20080609]
@serial 		int,
@acctime	int,
@Isvoted		tinyint,
@voteenable	tinyint,
@scaner	int

AS

Update [dbo].[tbl_Supplement]
Set AccumPlayTime = @acctime, IsVoted = @Isvoted, VoteEnable = @voteenable, ScanerCnt = @scaner
FROM [dbo].[tbl_Supplement]  
WHERE Serial = @serial




GO
/****** Object:  StoredProcedure [dbo].[pUpdate_TimeLimit_Info]    Script Date: 12/01/2026 16:57:20 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO



/****** 개체: 저장 프로시저 누적 피로도와 플레이어 상태를 치트를 사용해 수정 한다. ******/ 
Create Procedure [dbo].[pUpdate_TimeLimit_Info]
@AccSerial	int,
@Fatigue		int,
@Status		tinyint

AS

Update [dbo].[tbl_TimeLimitInfo]
Set Fatigue = @Fatigue, TLStatus = @Status
Where AccountSerial = @AccSerial




GO
/****** Object:  StoredProcedure [dbo].[pUpdate_UnitData]    Script Date: 12/01/2026 16:57:20 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO






create procedure [dbo].[pUpdate_UnitData]
@serial int,
@data0 float,
@data1 float
as
update [dbo].[tbl_unitdata] set Data0 = @data0, Data1 = @data1 where Serial = @serial







GO
/****** Object:  StoredProcedure [dbo].[pUpdate_UserGuildData]    Script Date: 12/01/2026 16:57:20 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO






--길드가입처리
CREATE PROCEDURE [dbo].[pUpdate_UserGuildData]
@serial int,
@guildSerial  int,
@grade int
AS
UPDATE [dbo].[tbl_general] SET GuildSerial=@guildSerial, GuildGrade = @grade  WHERE serial = @serial







GO
/****** Object:  StoredProcedure [dbo].[pUpdate_UserGuildData_20070614]    Script Date: 12/01/2026 16:57:20 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO




/*
   작성자 : 김태완
   작성일 : 2007-06-14 (목)
   내용 : 길드 위원회 추가로 기존 프로시져(pUpdate_UserGuildData) 변경 (GuildClass -> GuildStatus)
*/
CREATE PROCEDURE [dbo].[pUpdate_UserGuildData_20070614]
@serial int,
@guildSerial  int,
@grade int
AS
UPDATE [dbo].[tbl_general] SET GuildSerial=@guildSerial, GuildStatus = @grade  WHERE serial = @serial





GO
/****** Object:  StoredProcedure [dbo].[pUpdate_utcheatregdate]    Script Date: 12/01/2026 16:57:20 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO




/**
 @date 2007-04-27
 @author neon7
 @brief 무인거래기 등록 물품 해지 치트 사용시
 등록 날짜를 등록기간 지난 만큼 설정 한다.
 강제로 등록 시간 경과로 해지 될수 있게 설정 한다.
 */
create procedure [dbo].[pUpdate_utcheatregdate]
@type as tinyint,
@serial as int
as
update [dbo].[tbl_utsellinfo] set  [regdate]=dateadd( hh, -[sellturm], [regdate] ) where [type]=@type and [serial]=@serial






GO
/****** Object:  StoredProcedure [dbo].[pUpdate_utitemstate]    Script Date: 12/01/2026 16:57:20 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO







create procedure [dbo].[pUpdate_utitemstate]
@type tinyint,
@serial int,
@state tinyint,
@regdate datetime
as
update [dbo].[tbl_utresultinfo] set state = @state, resultdate = @regdate where type = @type and serial = @serial







GO
/****** Object:  StoredProcedure [dbo].[pUpdate_utresulttax]    Script Date: 12/01/2026 16:57:20 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO




/**
 @date 2007-04-27
 @author neon7
 @brief 등록된 물품 가격 변경시 세금 정보 갱신 한다.
 */
create procedure [dbo].[pUpdate_utresulttax]
@type tinyint,
@serial int,
@tax int,
@regdate datetime
as
update [dbo].[tbl_utresultinfo] set [tax]=@tax, [resultdate]= @regdate where [type]= @type and [serial]=@serial






GO
/****** Object:  StoredProcedure [dbo].[pUpdate_utresutlinfobuy]    Script Date: 12/01/2026 16:57:20 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO






create procedure [dbo].[pUpdate_utresutlinfobuy]
@type tinyint,
@serial int,
@state tinyint,
@buyer int,
@tax int,
@regdate datetime
as
update [dbo].[tbl_utresultinfo] set [state]=@state, [resultdate]=@regdate, [buyer]=@buyer, [tax]=@tax where [type]=@type and [serial]=@serial







GO
/****** Object:  StoredProcedure [dbo].[pUpdate_utsellinfo_reregist]    Script Date: 12/01/2026 16:57:20 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO




/**
 @date 2007-04-27
 @author neon7
 @brief 재등록 시 판매정보에 재등록 관련 정보 업데이트
 등록일, 판매가격 업데이트
 */
create procedure [dbo].[pUpdate_utsellinfo_reregist]
@type tinyint,
@serial int,  
@regdate datetime,
@price float
as
update [dbo].[tbl_utsellinfo] set [regdate]=@regdate, [price]=@price where [type]=@type and [serial]=@serial






GO
/****** Object:  StoredProcedure [dbo].[pUpdate_utsellinfoprice]    Script Date: 12/01/2026 16:57:20 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO



CREATE procedure [dbo].[pUpdate_utsellinfoprice]
@type tinyint,
@serial int, 
@owner int,
@price bigint
as
update [dbo].[tbl_utsellinfo] set [price] = @price where [type] = @type and [serial] = @serial and [owner] = @owner






GO
/****** Object:  StoredProcedure [dbo].[pUpdate_utsingleiteminfo]    Script Date: 12/01/2026 16:57:20 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO






create procedure [dbo].[pUpdate_utsingleiteminfo]
@serial int,
@inveninx tinyint,
@k int,
@d int,
@u int,
@lv tinyint,
@grade tinyint,
@class1 tinyint,
@class2 tinyint,
@class3 tinyint
as
update [dbo].[tbl_utsingleiteminfo] set [inveninx] = @inveninx, [k] = @k, [d] = @d, [u] = @u, [lv] = @lv, [grade] = @grade, [class1] = @class1, [class2] = @class2, [class3] = @class3 where [serial] = @serial







GO
/****** Object:  StoredProcedure [dbo].[pUpdate_utsingleiteminfo_20061115]    Script Date: 12/01/2026 16:57:20 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO





CREATE procedure [dbo].[pUpdate_utsingleiteminfo_20061115]
@serial int,
@inveninx tinyint,
@k int,
@d int,
@u int,
@lv tinyint,
@grade tinyint,
@class1 tinyint,
@class2 tinyint,
@class3 tinyint,
@uid bigint,
@tmR int
as
update [dbo].[tbl_utsingleiteminfo] set [inveninx] = @inveninx, [k] = @k, [d] = @d, [u] = @u, [lv] = @lv, [grade] = @grade, [class1] = @class1, [class2] = @class2, [class3] = @class3, [S]=@uid, [T]=@tmR
where [serial] = @serial






GO
/****** Object:  StoredProcedure [dbo].[pUpdate_utsingleiteminfo_20070601]    Script Date: 12/01/2026 16:57:20 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO



create procedure [dbo].[pUpdate_utsingleiteminfo_20070601]
@serial int,
@inveninx tinyint,
@k int,
@d bigint,
@u int,
@lv tinyint,
@grade tinyint,
@class1 tinyint,
@class2 tinyint,
@class3 tinyint,
@uid bigint,
@tmR int
as  
update [dbo].[tbl_utsingleiteminfo] set [inveninx] = @inveninx, [k] = @k, [d] = @d, [u] = @u, [lv] = @lv, [grade] = @grade, [class1] = @class1, [class2] = @class2, [class3] = @class3, [S]=@uid, [T]=@tmR  
where [serial] = @serial





GO
/****** Object:  StoredProcedure [dbo].[pUpdate_WeeklyGuildGuildBattlePVPPoint]    Script Date: 12/01/2026 16:57:20 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO






create procedure [dbo].[pUpdate_WeeklyGuildGuildBattlePVPPoint]  
@serial int,  
@point float  
as  
update [dbo].[tbl_WeeklyGuildPVPPointSum] set guildbattlepvppoint = guildbattlepvppoint + @point where serial = @serial







GO
/****** Object:  StoredProcedure [dbo].[pUpdate_WeeklyGuildKillPVPPoint]    Script Date: 12/01/2026 16:57:20 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO







create procedure [dbo].[pUpdate_WeeklyGuildKillPVPPoint]
@serial int,
@point float
as
update [dbo].[tbl_WeeklyGuildPVPPointSum] set killpvppoint = killpvppoint + @point where serial = @serial







GO
/****** Object:  StoredProcedure [dbo].[pUpdate_WinGuildBattleRank]    Script Date: 12/01/2026 16:57:20 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO







create procedure [dbo].[pUpdate_WinGuildBattleRank]  
@serial int,
@score int
as  
update [dbo].[tbl_GuildBattleRank] set win=win+1, score=score+@score,  totwin=totwin+1, totcnt=totcnt+1 where serial = @serial







GO
/****** Object:  StoredProcedure [dbo].[pUpdateClear_utresultinfo]    Script Date: 12/01/2026 16:57:20 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO




create procedure [dbo].[pUpdateClear_utresultinfo]
@serial int,
@updatedate datetime
as
update [dbo].[tbl_utresultinfo] set [state]=0, [resultdate]=@updatedate, [buyer]=0, [tax]=0
where [serial]=@serial






GO
/****** Object:  StoredProcedure [dbo].[pUpdateClear_utsellinfo]    Script Date: 12/01/2026 16:57:20 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO




create procedure [dbo].[pUpdateClear_utsellinfo]
@serial int,
@updatedate datetime
as
update [dbo].[tbl_utsellinfo] set [race]=255, [regdate]=@updatedate, [owner]=0, [price]=0, [sellturm]=0
where [serial]=@serial






GO
/****** Object:  StoredProcedure [dbo].[pUpdateClear_utsingleiteminfo]    Script Date: 12/01/2026 16:57:20 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO



create procedure [dbo].[pUpdateClear_utsingleiteminfo]
@serial int
as
update [dbo].[tbl_utsingleiteminfo] set [inveninx]=255, [k]=-1, [d]=0, [u]=0x0FFFFFFF, [lv]=0, [grade]=0, [class1]=255, [class2]=255, [class3]=255
where serial=@serial






GO
/****** Object:  StoredProcedure [dbo].[pUpdatet_Account_Vote_Available]    Script Date: 12/01/2026 16:57:20 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER OFF
GO


/****** 개체: 저장 프로시저 하나의 계정에 대한 누적 투표 가능 여부를 수정 한다.******/ 
CREATE   PROCEDURE [dbo].[pUpdatet_Account_Vote_Available]
@accserial 		 int,
@vote			tinyint 		output


As
SET NOCOUNT ON


declare @accum	 int
Set @accum = 0


Select @accum = s.IsVoted + @accum
From [dbo].[tbl_Supplement] as s join tbl_base as b on s.serial = b.serial
Where b.accountserial = @accserial AND b.DCK = 0

if(@accum = 0)
begin
	Update [dbo].[tbl_Supplement] 
	Set IsVoted = 1, VoteEnable = 0, @vote = 0
	From [dbo].[tbl_Supplement] as s join [dbo].[tbl_base] as b on s.serial = b.serial
	Where b.accountserial = @accserial AND b.DCK = 0
end


if(@accum <> 0)
begin
	Update [dbo].[tbl_Supplement] 
	Set IsVoted = 1, @vote = 0
	From [dbo].[tbl_Supplement] as s  join [dbo].[tbl_base] as b on s.serial = b.serial
	Where b.accountserial = @accserial AND b.DCK = 0
end


IF (@@ERROR <> 0 )BEGIN	
	RETURN @@ERROR
END

SET NOCOUNT OFF






GO
/****** Object:  StoredProcedure [dbo].[wSelect_General]    Script Date: 12/01/2026 16:57:20 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO








CREATE PROCEDURE [dbo].[wSelect_General]
@name varchar(17)
as 
select * from [dbo].[tbl_general] as g join [dbo].[tbl_base] as b on b.serial = g.serial
where b.name=@name



GO
USE [master]
GO
ALTER DATABASE [RF_World] SET  READ_WRITE 
GO
