-- ============================================================================
-- AUTOCURE - Auto Cure on Macro Columns
-- Reference: 6.23_Guard 2/Server/[SqlAddon]/AUTOCURE.sql
-- Target: RF_World database
-- ============================================================================
-- MCI = Macro Cure Item index (0xFFFFFFFF = none)
-- MCV = Macro Cure Value/mode (0 = disabled)
-- ============================================================================

USE [RF_World]
GO

IF NOT EXISTS (SELECT 1 FROM INFORMATION_SCHEMA.COLUMNS
               WHERE TABLE_NAME = 'tbl_base' AND COLUMN_NAME = 'MCI')
BEGIN
    ALTER TABLE [dbo].[tbl_base] ADD MCI INT NOT NULL DEFAULT 0xFFFFFFFF
    PRINT 'Added tbl_base.MCI (Macro Cure Item)'
END

IF NOT EXISTS (SELECT 1 FROM INFORMATION_SCHEMA.COLUMNS
               WHERE TABLE_NAME = 'tbl_base' AND COLUMN_NAME = 'MCV')
BEGIN
    ALTER TABLE [dbo].[tbl_base] ADD MCV TINYINT NOT NULL DEFAULT 0
    PRINT 'Added tbl_base.MCV (Macro Cure Value)'
END
GO

PRINT 'AutoCure columns ready'
GO
