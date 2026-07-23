-- ============================================================================
-- AUCTION_FIX - Unmanned Trader / Auction Fix
-- Reference: 6.23_Guard 2/Server/[SqlAddon]/AUCTION_FIX.sql
-- Target: RF_World database
-- ============================================================================
-- Removes auction disable constraint, fixes item state update and
-- cross-race auction select procedure
-- ============================================================================

USE [RF_World]
GO

-- Remove auction disable constraint if exists
IF EXISTS (SELECT CONSTRAINT_NAME FROM INFORMATION_SCHEMA.TABLE_CONSTRAINTS
           WHERE CONSTRAINT_NAME = 'CK_tbl_utsellinfo')
BEGIN
    ALTER TABLE dbo.tbl_utsellinfo DROP CONSTRAINT CK_tbl_utsellinfo
    PRINT 'Removed auction disable constraint CK_tbl_utsellinfo'
    PRINT '  (To disable auction, use config UnmannedTrader.ini instead)'
END
GO

-- Fix: Item state update procedure
-- When state = 3 (cancelled) or 5 (expired), reset result row fully
IF OBJECT_ID('dbo.pUpdate_utitemstate', 'P') IS NOT NULL
    DROP PROCEDURE [dbo].[pUpdate_utitemstate]
GO

CREATE PROCEDURE [dbo].[pUpdate_utitemstate]
    @type    TINYINT,
    @serial  INT,
    @state   TINYINT,
    @regdate DATETIME
AS
BEGIN
    SET NOCOUNT ON

    IF @state = 3 OR @state = 5
        UPDATE [dbo].[tbl_utresultinfo]
        SET [state] = 0, [resultdate] = @regdate, [buyer] = 0, [tax] = 0
        WHERE type = @type AND serial = @serial
    ELSE
        UPDATE [dbo].[tbl_utresultinfo]
        SET [state] = @state, [resultdate] = @regdate
        WHERE type = @type AND serial = @serial
END
GO

PRINT 'Created procedure: pUpdate_utitemstate'
GO

-- Fix: Cross-race auction select
-- When @race = 255, show all races (interracial auction)
-- Otherwise filter by specific race
IF OBJECT_ID('dbo.pSelect_utsellinfo', 'P') IS NOT NULL
    DROP PROCEDURE [dbo].[pSelect_utsellinfo]
GO

CREATE PROCEDURE [dbo].[pSelect_utsellinfo]
    @type   TINYINT,
    @serial INT,
    @race   TINYINT
AS
BEGIN
    SET NOCOUNT ON

    IF @type = 0
    BEGIN
        SELECT s.[price], s.[owner], b.[Race], b.[Dalant], g.[GuildSerial],
               b.[AccountSerial], b.[Account], b.[Name]
        FROM [dbo].[tbl_utsellinfo] AS s
        JOIN [dbo].[tbl_base] AS b ON
            ((@race <> 255 AND s.[type] = @type AND s.[serial] = @serial
              AND s.[race] = @race AND b.[Serial] = s.[owner] AND b.[dck] = 0)
            OR (s.[type] = @type AND s.[serial] = @serial
                AND b.[Serial] = s.[owner] AND b.[dck] = 0))
        JOIN [dbo].[tbl_general] AS g ON g.[Serial] = s.[owner]
    END
    ELSE
    BEGIN
        SELECT s.[price], s.[owner], b.[Race], 0, g.[GuildSerial],
               b.[AccountSerial], b.[Account], b.[Name]
        FROM [dbo].[tbl_utsellinfo] AS s
        JOIN [dbo].[tbl_base] AS b ON
            ((@race <> 255 AND s.[type] = @type AND s.[serial] = @serial
              AND s.[race] = @race AND b.[Serial] = s.[owner] AND b.[dck] = 0)
            OR (s.[type] = @type AND s.[serial] = @serial
                AND b.[Serial] = s.[owner] AND b.[dck] = 0))
        JOIN [dbo].[tbl_general] AS g ON g.[Serial] = s.[owner]
    END
END
GO

PRINT 'Created procedure: pSelect_utsellinfo (with interracial support)'
GO
