-- ============================================================================
-- NEW_FIX_2022 - BIGINT Migration for Time Columns
-- Reference: 6.23_Guard 2/Server/[SqlAddon]/NEW_FIX_2022.sql
-- Target: RF_World database
-- ============================================================================
-- Fixes Y2038 problem by migrating time columns from INT to BIGINT.
-- Affected tables: tbl_base, tbl_supplement, tbl_racebattle_log,
--                  tbl_TimeLimitInfo, tbl_npc_quest_history (Time1-Time70)
-- ============================================================================
-- WARNING: Run during maintenance. Large tables may lock for seconds.
-- ============================================================================

USE [RF_World]
GO

-- ---- tbl_base.LastConnTime ----
IF EXISTS (SELECT 1 FROM INFORMATION_SCHEMA.COLUMNS
           WHERE TABLE_NAME = 'tbl_base' AND COLUMN_NAME = 'LastConnTime' AND DATA_TYPE = 'int')
BEGIN
    IF EXISTS (SELECT 1 FROM sys.default_constraints dc
               JOIN sys.columns c ON dc.parent_object_id = c.object_id AND dc.parent_column_id = c.column_id
               JOIN sys.tables t ON dc.parent_object_id = t.object_id
               WHERE t.name = 'tbl_base' AND c.name = 'LastConnTime')
    BEGIN
        DECLARE @cn1 NVARCHAR(200)
        SELECT @cn1 = dc.name FROM sys.default_constraints dc
            JOIN sys.columns c ON dc.parent_object_id = c.object_id AND dc.parent_column_id = c.column_id
            JOIN sys.tables t ON dc.parent_object_id = t.object_id
            WHERE t.name = 'tbl_base' AND c.name = 'LastConnTime'
        EXEC('ALTER TABLE [dbo].[tbl_base] DROP CONSTRAINT ' + @cn1)
    END
    ALTER TABLE [dbo].[tbl_base] ALTER COLUMN [LastConnTime] BIGINT NOT NULL
    ALTER TABLE [dbo].[tbl_base] ADD CONSTRAINT [DF_tbl_base_LastConnTime] DEFAULT ((0)) FOR [LastConnTime]
    PRINT 'Migrated tbl_base.LastConnTime to BIGINT'
END
GO

-- ---- tbl_supplement.cristalbattledate ----
IF EXISTS (SELECT 1 FROM INFORMATION_SCHEMA.COLUMNS
           WHERE TABLE_NAME = 'tbl_supplement' AND COLUMN_NAME = 'cristalbattledate' AND DATA_TYPE = 'int')
BEGIN
    IF EXISTS (SELECT 1 FROM sys.default_constraints dc
               JOIN sys.columns c ON dc.parent_object_id = c.object_id AND dc.parent_column_id = c.column_id
               JOIN sys.tables t ON dc.parent_object_id = t.object_id
               WHERE t.name = 'tbl_supplement' AND c.name = 'cristalbattledate')
    BEGIN
        DECLARE @cn2 NVARCHAR(200)
        SELECT @cn2 = dc.name FROM sys.default_constraints dc
            JOIN sys.columns c ON dc.parent_object_id = c.object_id AND dc.parent_column_id = c.column_id
            JOIN sys.tables t ON dc.parent_object_id = t.object_id
            WHERE t.name = 'tbl_supplement' AND c.name = 'cristalbattledate'
        EXEC('ALTER TABLE [dbo].[tbl_supplement] DROP CONSTRAINT ' + @cn2)
    END
    ALTER TABLE [dbo].[tbl_supplement] ALTER COLUMN [cristalbattledate] BIGINT NOT NULL
    ALTER TABLE [dbo].[tbl_supplement] ADD CONSTRAINT [DF_tbl_supplement_cristalbattledate] DEFAULT ((0)) FOR [cristalbattledate]
    PRINT 'Migrated tbl_supplement.cristalbattledate to BIGINT'
END
GO

-- ---- tbl_supplement.BufEndTime ----
IF EXISTS (SELECT 1 FROM INFORMATION_SCHEMA.COLUMNS
           WHERE TABLE_NAME = 'tbl_supplement' AND COLUMN_NAME = 'BufEndTime' AND DATA_TYPE = 'int')
BEGIN
    IF EXISTS (SELECT 1 FROM sys.default_constraints dc
               JOIN sys.columns c ON dc.parent_object_id = c.object_id AND dc.parent_column_id = c.column_id
               JOIN sys.tables t ON dc.parent_object_id = t.object_id
               WHERE t.name = 'tbl_supplement' AND c.name = 'BufEndTime')
    BEGIN
        DECLARE @cn3 NVARCHAR(200)
        SELECT @cn3 = dc.name FROM sys.default_constraints dc
            JOIN sys.columns c ON dc.parent_object_id = c.object_id AND dc.parent_column_id = c.column_id
            JOIN sys.tables t ON dc.parent_object_id = t.object_id
            WHERE t.name = 'tbl_supplement' AND c.name = 'BufEndTime'
        EXEC('ALTER TABLE [dbo].[tbl_supplement] DROP CONSTRAINT ' + @cn3)
    END
    ALTER TABLE [dbo].[tbl_supplement] ALTER COLUMN [BufEndTime] BIGINT NOT NULL
    ALTER TABLE [dbo].[tbl_supplement] ADD CONSTRAINT [DF__tbl_supplement_BufEndTime] DEFAULT ((0)) FOR [BufEndTime]
    PRINT 'Migrated tbl_supplement.BufEndTime to BIGINT'
END
GO

-- ---- tbl_racebattle_log.endtime ----
IF EXISTS (SELECT 1 FROM INFORMATION_SCHEMA.COLUMNS
           WHERE TABLE_NAME = 'tbl_racebattle_log' AND COLUMN_NAME = 'endtime' AND DATA_TYPE = 'int')
BEGIN
    ALTER TABLE [dbo].[tbl_racebattle_log] ALTER COLUMN [endtime] BIGINT NOT NULL
    PRINT 'Migrated tbl_racebattle_log.endtime to BIGINT'
END
GO

-- ---- tbl_TimeLimitInfo.LastLogoutTime ----
IF EXISTS (SELECT 1 FROM INFORMATION_SCHEMA.COLUMNS
           WHERE TABLE_NAME = 'tbl_TimeLimitInfo' AND COLUMN_NAME = 'LastLogoutTime' AND DATA_TYPE = 'int')
BEGIN
    IF EXISTS (SELECT 1 FROM sys.default_constraints dc
               JOIN sys.columns c ON dc.parent_object_id = c.object_id AND dc.parent_column_id = c.column_id
               JOIN sys.tables t ON dc.parent_object_id = t.object_id
               WHERE t.name = 'tbl_TimeLimitInfo' AND c.name = 'LastLogoutTime')
    BEGIN
        DECLARE @cn4 NVARCHAR(200)
        SELECT @cn4 = dc.name FROM sys.default_constraints dc
            JOIN sys.columns c ON dc.parent_object_id = c.object_id AND dc.parent_column_id = c.column_id
            JOIN sys.tables t ON dc.parent_object_id = t.object_id
            WHERE t.name = 'tbl_TimeLimitInfo' AND c.name = 'LastLogoutTime'
        EXEC('ALTER TABLE [dbo].[tbl_TimeLimitInfo] DROP CONSTRAINT ' + @cn4)
    END
    ALTER TABLE [dbo].[tbl_TimeLimitInfo] ALTER COLUMN [LastLogoutTime] BIGINT NOT NULL
    ALTER TABLE [dbo].[tbl_TimeLimitInfo] ADD CONSTRAINT [DF_tbl_TimeLimitInfo_LastLoginTime] DEFAULT ((101010000)) FOR [LastLogoutTime]
    PRINT 'Migrated tbl_TimeLimitInfo.LastLogoutTime to BIGINT'
END
GO

-- ---- tbl_npc_quest_history.Time1 through Time70 ----
-- Drops all default constraints, alters to BIGINT, re-adds defaults
DECLARE @i INT = 1
DECLARE @colName NVARCHAR(20)
DECLARE @constraintName NVARCHAR(200)
DECLARE @sql NVARCHAR(MAX)

WHILE @i <= 70
BEGIN
    SET @colName = 'Time' + CAST(@i AS VARCHAR)

    IF EXISTS (SELECT 1 FROM INFORMATION_SCHEMA.COLUMNS
               WHERE TABLE_NAME = 'tbl_npc_quest_history' AND COLUMN_NAME = @colName AND DATA_TYPE = 'int')
    BEGIN
        -- Drop existing default constraint
        SELECT @constraintName = dc.name
        FROM sys.default_constraints dc
        JOIN sys.columns c ON dc.parent_object_id = c.object_id AND dc.parent_column_id = c.column_id
        JOIN sys.tables t ON dc.parent_object_id = t.object_id
        WHERE t.name = 'tbl_npc_quest_history' AND c.name = @colName

        IF @constraintName IS NOT NULL
        BEGIN
            SET @sql = 'ALTER TABLE [dbo].[tbl_npc_quest_history] DROP CONSTRAINT ' + QUOTENAME(@constraintName)
            EXEC sp_executesql @sql
        END

        -- Alter to BIGINT
        SET @sql = 'ALTER TABLE [dbo].[tbl_npc_quest_history] ALTER COLUMN ' + QUOTENAME(@colName) + ' BIGINT NOT NULL'
        EXEC sp_executesql @sql

        -- Re-add default
        SET @sql = 'ALTER TABLE [dbo].[tbl_npc_quest_history] ADD CONSTRAINT [DF_tbl_npc_quest_' + @colName + '] DEFAULT ((0)) FOR ' + QUOTENAME(@colName)
        EXEC sp_executesql @sql

        SET @constraintName = NULL
    END

    SET @i = @i + 1
END

PRINT 'Migrated tbl_npc_quest_history.Time1-Time70 to BIGINT'
GO
