-- ============================================================================
-- RF Online Protection System - Auction & Economy Fixes
-- Version: 1.0
-- Description: Fixes integer overflow vulnerabilities in gold/dalant columns,
--              adds constraints to prevent negative currency exploits
-- Target: MSSQL Server (RF Online Game Database)
-- ============================================================================
-- WARNING: These ALTER statements may take time on large tables.
-- Run during maintenance window. BACKUP FIRST!
-- ============================================================================

-- ============================================================================
-- 1. Fix Gold/Dalant Integer Overflow
-- Original columns use INT (max 2,147,483,647). On high-rate servers with
-- inflated economies, this overflows to negative values. Change to BIGINT.
-- ============================================================================

-- Character gold (tbl_general or tbl_base depending on your DB version)
-- Adjust table/column names to match YOUR database schema.
-- Common table names: tbl_general, tbl_base, tbl_rfcharacter

PRINT '=== Checking gold/dalant columns for BIGINT migration ==='

-- tbl_general: Gold column
IF EXISTS (SELECT * FROM INFORMATION_SCHEMA.COLUMNS
           WHERE TABLE_NAME = 'tbl_general' AND COLUMN_NAME = 'Gold'
           AND DATA_TYPE = 'int')
BEGIN
    ALTER TABLE [dbo].[tbl_general] ALTER COLUMN [Gold] BIGINT NOT NULL
    PRINT 'Migrated tbl_general.Gold to BIGINT'
END

-- tbl_general: Dalant column
IF EXISTS (SELECT * FROM INFORMATION_SCHEMA.COLUMNS
           WHERE TABLE_NAME = 'tbl_general' AND COLUMN_NAME = 'Dalant'
           AND DATA_TYPE = 'int')
BEGIN
    ALTER TABLE [dbo].[tbl_general] ALTER COLUMN [Dalant] BIGINT NOT NULL
    PRINT 'Migrated tbl_general.Dalant to BIGINT'
END

-- Trunk (bank/storage) gold
IF EXISTS (SELECT * FROM INFORMATION_SCHEMA.COLUMNS
           WHERE TABLE_NAME = 'tbl_general' AND COLUMN_NAME = 'TrunkGold'
           AND DATA_TYPE = 'int')
BEGIN
    ALTER TABLE [dbo].[tbl_general] ALTER COLUMN [TrunkGold] BIGINT NOT NULL
    PRINT 'Migrated tbl_general.TrunkGold to BIGINT'
END

IF EXISTS (SELECT * FROM INFORMATION_SCHEMA.COLUMNS
           WHERE TABLE_NAME = 'tbl_general' AND COLUMN_NAME = 'TrunkDalant'
           AND DATA_TYPE = 'int')
BEGIN
    ALTER TABLE [dbo].[tbl_general] ALTER COLUMN [TrunkDalant] BIGINT NOT NULL
    PRINT 'Migrated tbl_general.TrunkDalant to BIGINT'
END
GO

-- ============================================================================
-- 2. Add CHECK constraints to prevent negative currency
-- These prevent gold duplication via integer underflow exploits
-- ============================================================================

PRINT '=== Adding CHECK constraints for negative currency prevention ==='

-- Only add if column exists and constraint doesn't already exist
IF EXISTS (SELECT * FROM INFORMATION_SCHEMA.COLUMNS
           WHERE TABLE_NAME = 'tbl_general' AND COLUMN_NAME = 'Gold')
AND NOT EXISTS (SELECT * FROM sys.check_constraints
                WHERE name = 'CK_general_gold_positive')
BEGIN
    ALTER TABLE [dbo].[tbl_general]
        ADD CONSTRAINT [CK_general_gold_positive] CHECK ([Gold] >= 0)
    PRINT 'Added CHECK constraint: Gold >= 0'
END

IF EXISTS (SELECT * FROM INFORMATION_SCHEMA.COLUMNS
           WHERE TABLE_NAME = 'tbl_general' AND COLUMN_NAME = 'Dalant')
AND NOT EXISTS (SELECT * FROM sys.check_constraints
                WHERE name = 'CK_general_dalant_positive')
BEGIN
    ALTER TABLE [dbo].[tbl_general]
        ADD CONSTRAINT [CK_general_dalant_positive] CHECK ([Dalant] >= 0)
    PRINT 'Added CHECK constraint: Dalant >= 0'
END

IF EXISTS (SELECT * FROM INFORMATION_SCHEMA.COLUMNS
           WHERE TABLE_NAME = 'tbl_general' AND COLUMN_NAME = 'TrunkGold')
AND NOT EXISTS (SELECT * FROM sys.check_constraints
                WHERE name = 'CK_general_trunkgold_positive')
BEGIN
    ALTER TABLE [dbo].[tbl_general]
        ADD CONSTRAINT [CK_general_trunkgold_positive] CHECK ([TrunkGold] >= 0)
    PRINT 'Added CHECK constraint: TrunkGold >= 0'
END

IF EXISTS (SELECT * FROM INFORMATION_SCHEMA.COLUMNS
           WHERE TABLE_NAME = 'tbl_general' AND COLUMN_NAME = 'TrunkDalant')
AND NOT EXISTS (SELECT * FROM sys.check_constraints
                WHERE name = 'CK_general_trunkdalant_positive')
BEGIN
    ALTER TABLE [dbo].[tbl_general]
        ADD CONSTRAINT [CK_general_trunkdalant_positive] CHECK ([TrunkDalant] >= 0)
    PRINT 'Added CHECK constraint: TrunkDalant >= 0'
END
GO

-- ============================================================================
-- 3. Fix negative gold in existing data
-- Clean up any rows that already have negative values from past exploits
-- ============================================================================

PRINT '=== Fixing negative currency values in existing data ==='

-- Fix negative Gold (set to 0)
IF EXISTS (SELECT * FROM INFORMATION_SCHEMA.COLUMNS
           WHERE TABLE_NAME = 'tbl_general' AND COLUMN_NAME = 'Gold')
BEGIN
    UPDATE [dbo].[tbl_general] SET [Gold] = 0 WHERE [Gold] < 0
    PRINT 'Fixed negative Gold values'
END

IF EXISTS (SELECT * FROM INFORMATION_SCHEMA.COLUMNS
           WHERE TABLE_NAME = 'tbl_general' AND COLUMN_NAME = 'Dalant')
BEGIN
    UPDATE [dbo].[tbl_general] SET [Dalant] = 0 WHERE [Dalant] < 0
    PRINT 'Fixed negative Dalant values'
END

IF EXISTS (SELECT * FROM INFORMATION_SCHEMA.COLUMNS
           WHERE TABLE_NAME = 'tbl_general' AND COLUMN_NAME = 'TrunkGold')
BEGIN
    UPDATE [dbo].[tbl_general] SET [TrunkGold] = 0 WHERE [TrunkGold] < 0
    PRINT 'Fixed negative TrunkGold values'
END

IF EXISTS (SELECT * FROM INFORMATION_SCHEMA.COLUMNS
           WHERE TABLE_NAME = 'tbl_general' AND COLUMN_NAME = 'TrunkDalant')
BEGIN
    UPDATE [dbo].[tbl_general] SET [TrunkDalant] = 0 WHERE [TrunkDalant] < 0
    PRINT 'Fixed negative TrunkDalant values'
END
GO
