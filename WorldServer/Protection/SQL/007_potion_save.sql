-- ============================================================================
-- POTION_SAVE - Persist Potion Buffs Across Logout/Disconnect
-- Reference: 6.23_Guard 2/Server/[SqlAddon]/POTION_SAVE.sql
-- Target: RF_World database
-- ============================================================================
-- Adds columns to tbl_base to store active potion index and remaining duration
-- PI0/PI1 = Potion Item Index (-1 = none)
-- PD0/PD1 = Potion Duration remaining (seconds)
-- ============================================================================

USE [RF_World]
GO

IF NOT EXISTS (SELECT 1 FROM INFORMATION_SCHEMA.COLUMNS
               WHERE TABLE_NAME = 'tbl_base' AND COLUMN_NAME = 'PI0')
BEGIN
    ALTER TABLE [dbo].[tbl_base] ADD PI0 INT NOT NULL DEFAULT -1
    PRINT 'Added tbl_base.PI0 (Potion Index slot 0)'
END

IF NOT EXISTS (SELECT 1 FROM INFORMATION_SCHEMA.COLUMNS
               WHERE TABLE_NAME = 'tbl_base' AND COLUMN_NAME = 'PD0')
BEGIN
    ALTER TABLE [dbo].[tbl_base] ADD PD0 INT NOT NULL DEFAULT 0
    PRINT 'Added tbl_base.PD0 (Potion Duration slot 0)'
END

IF NOT EXISTS (SELECT 1 FROM INFORMATION_SCHEMA.COLUMNS
               WHERE TABLE_NAME = 'tbl_base' AND COLUMN_NAME = 'PI1')
BEGIN
    ALTER TABLE [dbo].[tbl_base] ADD PI1 INT NOT NULL DEFAULT -1
    PRINT 'Added tbl_base.PI1 (Potion Index slot 1)'
END

IF NOT EXISTS (SELECT 1 FROM INFORMATION_SCHEMA.COLUMNS
               WHERE TABLE_NAME = 'tbl_base' AND COLUMN_NAME = 'PD1')
BEGIN
    ALTER TABLE [dbo].[tbl_base] ADD PD1 INT NOT NULL DEFAULT 0
    PRINT 'Added tbl_base.PD1 (Potion Duration slot 1)'
END
GO

PRINT 'Potion Save columns ready'
GO
