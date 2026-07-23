-- ============================================================================
-- GUILD_POINT - Guild Point System
-- Reference: 6.23_Guard 2/Server/[SqlAddon]/GUILD_POINT.sql
-- Target: RF_World database
-- ============================================================================

USE [RF_World]
GO

IF NOT EXISTS (SELECT 1 FROM INFORMATION_SCHEMA.COLUMNS
               WHERE TABLE_NAME = 'tbl_Guild' AND COLUMN_NAME = 'Point')
BEGIN
    ALTER TABLE [dbo].[tbl_Guild] ADD Point BIGINT NOT NULL DEFAULT(0)
    PRINT 'Added tbl_Guild.Point (Guild Point system)'
END
GO
