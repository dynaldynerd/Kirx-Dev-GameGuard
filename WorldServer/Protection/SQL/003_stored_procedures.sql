-- ============================================================================
-- RF Online Protection System - Stored Procedures
-- Version: 1.0
-- Description: Stored procedures for HWID management, cheat logging,
--              and protection system queries
-- Target: MSSQL Server (RF Online Game Database)
-- ============================================================================

-- ============================================================================
-- 1. HWID Registration / Update
-- Called when a player connects with their hardware ID
-- ============================================================================

IF EXISTS (SELECT * FROM sysobjects WHERE name = 'sp_protect_hwid_register' AND type = 'P')
    DROP PROCEDURE [dbo].[sp_protect_hwid_register]
GO

CREATE PROCEDURE [dbo].[sp_protect_hwid_register]
    @hwid           VARCHAR(128),
    @account_id     VARCHAR(32),
    @char_name      NVARCHAR(20),
    @char_serial    INT
AS
BEGIN
    SET NOCOUNT ON

    IF EXISTS (SELECT 1 FROM [dbo].[tbl_protect_hwid] WHERE [hwid] = @hwid)
    BEGIN
        UPDATE [dbo].[tbl_protect_hwid]
        SET [account_id] = @account_id,
            [char_name] = @char_name,
            [char_serial] = @char_serial,
            [last_seen] = GETDATE()
        WHERE [hwid] = @hwid
    END
    ELSE
    BEGIN
        INSERT INTO [dbo].[tbl_protect_hwid]
            ([hwid], [account_id], [char_name], [char_serial])
        VALUES
            (@hwid, @account_id, @char_name, @char_serial)
    END

    -- Return block status
    SELECT [is_blocked], [block_reason]
    FROM [dbo].[tbl_protect_hwid]
    WHERE [hwid] = @hwid
END
GO

PRINT 'Created procedure: sp_protect_hwid_register'
GO

-- ============================================================================
-- 2. HWID Block/Unblock
-- Used by GM commands to block or unblock hardware IDs
-- ============================================================================

IF EXISTS (SELECT * FROM sysobjects WHERE name = 'sp_protect_hwid_block' AND type = 'P')
    DROP PROCEDURE [dbo].[sp_protect_hwid_block]
GO

CREATE PROCEDURE [dbo].[sp_protect_hwid_block]
    @hwid           VARCHAR(128),
    @block          TINYINT,       -- 1 = block, 0 = unblock
    @reason         NVARCHAR(256),
    @blocked_by     VARCHAR(32)
AS
BEGIN
    SET NOCOUNT ON

    IF NOT EXISTS (SELECT 1 FROM [dbo].[tbl_protect_hwid] WHERE [hwid] = @hwid)
    BEGIN
        -- Insert new entry if blocking unknown HWID
        IF @block = 1
        BEGIN
            INSERT INTO [dbo].[tbl_protect_hwid]
                ([hwid], [is_blocked], [block_reason], [blocked_date], [blocked_by])
            VALUES
                (@hwid, 1, @reason, GETDATE(), @blocked_by)
        END
        RETURN
    END

    UPDATE [dbo].[tbl_protect_hwid]
    SET [is_blocked] = @block,
        [block_reason] = CASE WHEN @block = 1 THEN @reason ELSE NULL END,
        [blocked_date] = CASE WHEN @block = 1 THEN GETDATE() ELSE NULL END,
        [blocked_by] = CASE WHEN @block = 1 THEN @blocked_by ELSE NULL END
    WHERE [hwid] = @hwid
END
GO

PRINT 'Created procedure: sp_protect_hwid_block'
GO

-- ============================================================================
-- 3. Log Cheat Event
-- Called by the Protection System when a cheat is detected
-- ============================================================================

IF EXISTS (SELECT * FROM sysobjects WHERE name = 'sp_protect_log_cheat' AND type = 'P')
    DROP PROCEDURE [dbo].[sp_protect_log_cheat]
GO

CREATE PROCEDURE [dbo].[sp_protect_log_cheat]
    @char_serial    INT,
    @char_name      NVARCHAR(20),
    @account_id     VARCHAR(32),
    @hwid           VARCHAR(128),
    @cheat_type     VARCHAR(32),
    @score_added    INT,
    @total_score    INT,
    @action_taken   TINYINT,
    @detail         NVARCHAR(512),
    @pos_x          FLOAT = NULL,
    @pos_y          FLOAT = NULL,
    @pos_z          FLOAT = NULL,
    @map_index      INT = NULL
AS
BEGIN
    SET NOCOUNT ON

    INSERT INTO [dbo].[tbl_protect_cheat_log]
        ([char_serial], [char_name], [account_id], [hwid],
         [cheat_type], [score_added], [total_score], [action_taken],
         [detail], [pos_x], [pos_y], [pos_z], [map_index])
    VALUES
        (@char_serial, @char_name, @account_id, @hwid,
         @cheat_type, @score_added, @total_score, @action_taken,
         @detail, @pos_x, @pos_y, @pos_z, @map_index)
END
GO

PRINT 'Created procedure: sp_protect_log_cheat'
GO

-- ============================================================================
-- 4. Get Blocked HWIDs
-- Used at server startup to load the blocked HWID list into memory
-- ============================================================================

IF EXISTS (SELECT * FROM sysobjects WHERE name = 'sp_protect_get_blocked_hwids' AND type = 'P')
    DROP PROCEDURE [dbo].[sp_protect_get_blocked_hwids]
GO

CREATE PROCEDURE [dbo].[sp_protect_get_blocked_hwids]
AS
BEGIN
    SET NOCOUNT ON

    SELECT [hwid], [block_reason], [blocked_date], [blocked_by]
    FROM [dbo].[tbl_protect_hwid]
    WHERE [is_blocked] = 1
END
GO

PRINT 'Created procedure: sp_protect_get_blocked_hwids'
GO

-- ============================================================================
-- 5. Cheat Report Summary
-- GM tool: get summary of cheat detections by type in a date range
-- ============================================================================

IF EXISTS (SELECT * FROM sysobjects WHERE name = 'sp_protect_cheat_report' AND type = 'P')
    DROP PROCEDURE [dbo].[sp_protect_cheat_report]
GO

CREATE PROCEDURE [dbo].[sp_protect_cheat_report]
    @date_from      DATETIME = NULL,
    @date_to        DATETIME = NULL
AS
BEGIN
    SET NOCOUNT ON

    IF @date_from IS NULL SET @date_from = DATEADD(DAY, -7, GETDATE())
    IF @date_to IS NULL SET @date_to = GETDATE()

    -- Summary by type
    SELECT
        [cheat_type],
        COUNT(*) AS [total_events],
        COUNT(DISTINCT [char_serial]) AS [unique_players],
        SUM(CASE WHEN [action_taken] = 1 THEN 1 ELSE 0 END) AS [warnings],
        SUM(CASE WHEN [action_taken] = 2 THEN 1 ELSE 0 END) AS [kicks],
        SUM(CASE WHEN [action_taken] = 3 THEN 1 ELSE 0 END) AS [bans]
    FROM [dbo].[tbl_protect_cheat_log]
    WHERE [log_time] BETWEEN @date_from AND @date_to
    GROUP BY [cheat_type]
    ORDER BY [total_events] DESC

    -- Top offenders
    SELECT TOP 20
        [char_serial],
        MAX([char_name]) AS [char_name],
        MAX([account_id]) AS [account_id],
        COUNT(*) AS [total_events],
        MAX([total_score]) AS [peak_score],
        MAX([action_taken]) AS [worst_action]
    FROM [dbo].[tbl_protect_cheat_log]
    WHERE [log_time] BETWEEN @date_from AND @date_to
    GROUP BY [char_serial]
    ORDER BY [total_events] DESC
END
GO

PRINT 'Created procedure: sp_protect_cheat_report'
GO

-- ============================================================================
-- 6. Cleanup old logs (run periodically via SQL Agent job)
-- Keeps logs for 90 days by default
-- ============================================================================

IF EXISTS (SELECT * FROM sysobjects WHERE name = 'sp_protect_cleanup_logs' AND type = 'P')
    DROP PROCEDURE [dbo].[sp_protect_cleanup_logs]
GO

CREATE PROCEDURE [dbo].[sp_protect_cleanup_logs]
    @retention_days  INT = 90
AS
BEGIN
    SET NOCOUNT ON

    DECLARE @cutoff DATETIME = DATEADD(DAY, -@retention_days, GETDATE())

    DELETE FROM [dbo].[tbl_protect_cheat_log]
    WHERE [log_time] < @cutoff

    PRINT 'Deleted cheat logs older than ' + CAST(@retention_days AS VARCHAR) + ' days'

    DELETE FROM [dbo].[tbl_protect_transaction_log]
    WHERE [log_time] < @cutoff

    PRINT 'Deleted transaction logs older than ' + CAST(@retention_days AS VARCHAR) + ' days'
END
GO

PRINT 'Created procedure: sp_protect_cleanup_logs'
GO

PRINT ''
PRINT '============================================'
PRINT ' All Protection System SQL objects created!'
PRINT '============================================'
GO
