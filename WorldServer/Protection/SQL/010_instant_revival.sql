-- ============================================================================
-- INSTANT_REVIVAL - Instant Revival Cooldown Tracking
-- Reference: 6.23_Guard 2/Server/[SqlAddon]/INSTAN_REVIVAL.sql
-- Target: RF_World database
-- ============================================================================
-- IR0 = Revival timestamp slot 0 (BIGINT for Y2038 safety)
-- IR1 = Revival timestamp slot 1
-- ============================================================================

USE [RF_World]
GO

-- IR0
DECLARE @cn0 NVARCHAR(200)
IF EXISTS (SELECT 1 FROM INFORMATION_SCHEMA.COLUMNS
           WHERE TABLE_NAME = 'tbl_base' AND COLUMN_NAME = 'IR0')
BEGIN
    -- Column exists, migrate to BIGINT if needed
    IF EXISTS (SELECT 1 FROM INFORMATION_SCHEMA.COLUMNS
               WHERE TABLE_NAME = 'tbl_base' AND COLUMN_NAME = 'IR0' AND DATA_TYPE = 'int')
    BEGIN
        SELECT @cn0 = dc.name FROM sys.default_constraints dc
            JOIN sys.columns c ON dc.parent_object_id = c.object_id AND dc.parent_column_id = c.column_id
            JOIN sys.tables t ON dc.parent_object_id = t.object_id
            WHERE t.name = 'tbl_base' AND c.name = 'IR0'
        IF @cn0 IS NOT NULL
            EXEC('ALTER TABLE [dbo].[tbl_base] DROP CONSTRAINT ' + @cn0)

        ALTER TABLE [dbo].[tbl_base] ALTER COLUMN IR0 BIGINT NOT NULL
        ALTER TABLE [dbo].[tbl_base] ADD CONSTRAINT DF_tbl_base_IR0 DEFAULT 0 FOR IR0
        PRINT 'Migrated tbl_base.IR0 to BIGINT'
    END
END
ELSE
BEGIN
    ALTER TABLE [dbo].[tbl_base] ADD IR0 BIGINT NOT NULL DEFAULT 0
    PRINT 'Added tbl_base.IR0 (Instant Revival slot 0)'
END
GO

-- IR1
DECLARE @cn1 NVARCHAR(200)
IF EXISTS (SELECT 1 FROM INFORMATION_SCHEMA.COLUMNS
           WHERE TABLE_NAME = 'tbl_base' AND COLUMN_NAME = 'IR1')
BEGIN
    IF EXISTS (SELECT 1 FROM INFORMATION_SCHEMA.COLUMNS
               WHERE TABLE_NAME = 'tbl_base' AND COLUMN_NAME = 'IR1' AND DATA_TYPE = 'int')
    BEGIN
        SELECT @cn1 = dc.name FROM sys.default_constraints dc
            JOIN sys.columns c ON dc.parent_object_id = c.object_id AND dc.parent_column_id = c.column_id
            JOIN sys.tables t ON dc.parent_object_id = t.object_id
            WHERE t.name = 'tbl_base' AND c.name = 'IR1'
        IF @cn1 IS NOT NULL
            EXEC('ALTER TABLE [dbo].[tbl_base] DROP CONSTRAINT ' + @cn1)

        ALTER TABLE [dbo].[tbl_base] ALTER COLUMN IR1 BIGINT NOT NULL
        ALTER TABLE [dbo].[tbl_base] ADD CONSTRAINT DF_tbl_base_IR1 DEFAULT 0 FOR IR1
        PRINT 'Migrated tbl_base.IR1 to BIGINT'
    END
END
ELSE
BEGIN
    ALTER TABLE [dbo].[tbl_base] ADD IR1 BIGINT NOT NULL DEFAULT 0
    PRINT 'Added tbl_base.IR1 (Instant Revival slot 1)'
END
GO
