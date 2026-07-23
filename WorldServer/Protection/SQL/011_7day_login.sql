-- ============================================================================
-- 7DAY_LOGIN - 7-Day Login Reward System
-- Reference: 6.23_Guard 2/Server/[SqlAddon]/7DAY_LOGIN/7DAY_LOGIN.sql
-- Target: RF_World database
-- ============================================================================
-- Serial = Character serial (PK)
-- VIP = VIP status flag
-- LT0/LT1 = Last login timestamps (BIGINT)
-- CD0/CD1 = Consecutive day counters
-- ============================================================================

USE [RF_World]
GO

IF NOT EXISTS (SELECT * FROM sys.objects
               WHERE object_id = OBJECT_ID(N'[dbo].[tbl_7daylogin]') AND type IN (N'U'))
BEGIN
    CREATE TABLE [dbo].[tbl_7daylogin](
        [Serial] INT      NOT NULL,
        [VIP]    TINYINT  NOT NULL CONSTRAINT [DF_tbl_7daylogin_VIP] DEFAULT ((0)),
        [LT0]    BIGINT   NOT NULL CONSTRAINT [DF_tbl_7daylogin_LT0] DEFAULT ((0)),
        [CD0]    TINYINT  NOT NULL CONSTRAINT [DF_tbl_7daylogin_CD0] DEFAULT ((0)),
        [LT1]    BIGINT   NOT NULL CONSTRAINT [DF_tbl_7daylogin_LT1] DEFAULT ((0)),
        [CD1]    TINYINT  NOT NULL CONSTRAINT [DF_tbl_7daylogin_CD1] DEFAULT ((0)),
        CONSTRAINT [PK_tbl_7daylogin] PRIMARY KEY CLUSTERED ([Serial] ASC)
    )
    PRINT 'Created table: tbl_7daylogin'
END
ELSE
    PRINT 'Table tbl_7daylogin already exists'
GO

-- Select/auto-insert procedure
IF OBJECT_ID('dbo.pSelect_7daylogin', 'P') IS NOT NULL
    DROP PROCEDURE dbo.pSelect_7daylogin
GO

CREATE PROCEDURE [dbo].[pSelect_7daylogin]
    @serial INT
AS
BEGIN
    SET NOCOUNT ON

    IF NOT EXISTS (SELECT 1 FROM [dbo].[tbl_7daylogin] WHERE Serial = @serial)
        INSERT INTO [dbo].[tbl_7daylogin]([Serial]) VALUES (@serial)

    SELECT [VIP], [LT0], [CD0], [LT1], [CD1]
    FROM [dbo].[tbl_7daylogin]
    WHERE Serial = @serial
END
GO

PRINT 'Created procedure: pSelect_7daylogin'
GO

-- ============================================================================
-- 7DAY_LOGIN_RESET - Reset all login rewards
-- Reference: 6.23_Guard 2/Server/[SqlAddon]/7DAY_LOGIN/7DAY_LOGIN_RESET.sql
-- Run manually when you want to reset all players' login streaks
-- ============================================================================
-- To reset: uncomment and run:
-- UPDATE [dbo].[tbl_7daylogin] SET [VIP]=0, [LT0]=0, [LT1]=0, [CD0]=0, [CD1]=0
-- ============================================================================
