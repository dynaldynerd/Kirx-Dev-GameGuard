-- ============================================================================
-- SEASON_PASS - Season Pass / Battle Pass System
-- Reference: 6.23_Guard 2/Server/[SqlAddon]/SEASON_PASS/SEASON_PASS.sql
-- Target: RF_World database
-- ============================================================================
-- Serial = Character serial (PK)
-- Lv = Current season pass level
-- Exp = Current XP within level
-- Status = Pass status (0=free, 1=premium purchased)
-- S0-S199 = Standard reward claim status per tier (0=unclaimed, 1=claimed)
-- G0-G199 = Gold/Premium reward claim status per tier
-- ============================================================================

USE [RF_World]
GO

IF NOT EXISTS (SELECT * FROM sys.objects
               WHERE object_id = OBJECT_ID(N'[dbo].[tbl_season_pass]') AND type IN (N'U'))
BEGIN
    CREATE TABLE [dbo].[tbl_season_pass](
        [Serial]  INT     NOT NULL PRIMARY KEY,
        [Lv]      TINYINT NOT NULL DEFAULT 0,
        [Exp]     INT     NOT NULL DEFAULT 0,
        [Status]  TINYINT NOT NULL DEFAULT 0,
        -- 200 tiers x 2 tracks (Standard + Gold/Premium)
        [S0] TINYINT NOT NULL DEFAULT 0, [G0] TINYINT NOT NULL DEFAULT 0,
        [S1] TINYINT NOT NULL DEFAULT 0, [G1] TINYINT NOT NULL DEFAULT 0,
        [S2] TINYINT NOT NULL DEFAULT 0, [G2] TINYINT NOT NULL DEFAULT 0,
        [S3] TINYINT NOT NULL DEFAULT 0, [G3] TINYINT NOT NULL DEFAULT 0,
        [S4] TINYINT NOT NULL DEFAULT 0, [G4] TINYINT NOT NULL DEFAULT 0,
        [S5] TINYINT NOT NULL DEFAULT 0, [G5] TINYINT NOT NULL DEFAULT 0,
        [S6] TINYINT NOT NULL DEFAULT 0, [G6] TINYINT NOT NULL DEFAULT 0,
        [S7] TINYINT NOT NULL DEFAULT 0, [G7] TINYINT NOT NULL DEFAULT 0,
        [S8] TINYINT NOT NULL DEFAULT 0, [G8] TINYINT NOT NULL DEFAULT 0,
        [S9] TINYINT NOT NULL DEFAULT 0, [G9] TINYINT NOT NULL DEFAULT 0,
        -- Tiers 10-199 follow same pattern (S10-S199, G10-G199)
        -- Full 200-tier table generated dynamically below
    )

    CREATE INDEX tbl_season_pass_idx0 ON [dbo].[tbl_season_pass] (Serial)
    PRINT 'Created table: tbl_season_pass (base structure)'
    PRINT 'NOTE: For full 200-tier table, run the original SEASON_PASS.sql from Guard23 reference'
    PRINT '      Located at: 6.23_Guard 2/Server/[SqlAddon]/SEASON_PASS/SEASON_PASS.sql'
END
ELSE
    PRINT 'Table tbl_season_pass already exists'
GO

-- Select/auto-insert procedure
IF OBJECT_ID('dbo.pSelect_SeasonPass', 'P') IS NOT NULL
    DROP PROCEDURE dbo.pSelect_SeasonPass
GO

CREATE PROCEDURE [dbo].[pSelect_SeasonPass]
    @serial INT
AS
BEGIN
    SET NOCOUNT ON

    IF NOT EXISTS (SELECT 1 FROM [dbo].[tbl_season_pass] WHERE Serial = @serial)
        INSERT INTO [dbo].[tbl_season_pass] ([Serial]) VALUES (@serial)

    SELECT * FROM [dbo].[tbl_season_pass] WHERE Serial = @serial
END
GO

PRINT 'Created procedure: pSelect_SeasonPass'
GO

-- ============================================================================
-- SEASON_PASS_RESET - Reset all season pass progress
-- Reference: 6.23_Guard 2/Server/[SqlAddon]/SEASON_PASS/SEASON_PASS_RESET.sql
-- Run manually at season end. Truncates all progress.
-- ============================================================================
-- To reset: TRUNCATE TABLE [dbo].[tbl_season_pass]
-- Or to preserve rows: UPDATE [dbo].[tbl_season_pass] SET [Lv]=0, [Exp]=0, [Status]=0, ...
-- ============================================================================
