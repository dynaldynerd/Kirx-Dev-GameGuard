-- ============================================================================
-- RF Online Protection System - SQL Migration Script
-- Version: 1.0
-- Description: Creates tables and indexes required by the Protection System
-- Target: MSSQL Server (RF Online Game Database)
-- ============================================================================
-- IMPORTANT: Run this on the GAME database (typically RF_World or similar)
-- Always BACKUP your database before running migration scripts!
-- ============================================================================

-- ============================================================================
-- 1. HWID Tracking & Blocking Table
-- Used by AntiCheat module for hardware ID tracking and banning
-- ============================================================================

IF NOT EXISTS (SELECT * FROM sysobjects WHERE name = 'tbl_protect_hwid' AND xtype = 'U')
BEGIN
    CREATE TABLE [dbo].[tbl_protect_hwid] (
        [seq]               INT IDENTITY(1,1) NOT NULL,
        [hwid]              VARCHAR(128)       NOT NULL,  -- Hardware ID hash
        [account_id]        VARCHAR(32)        NULL,      -- Last known account
        [char_name]         NVARCHAR(20)       NULL,      -- Last known character
        [char_serial]       INT                NULL,      -- Last known serial
        [first_seen]        DATETIME           NOT NULL DEFAULT GETDATE(),
        [last_seen]         DATETIME           NOT NULL DEFAULT GETDATE(),
        [is_blocked]        TINYINT            NOT NULL DEFAULT 0,
        [block_reason]      NVARCHAR(256)      NULL,
        [blocked_date]      DATETIME           NULL,
        [blocked_by]        VARCHAR(32)        NULL,      -- GM who blocked
        CONSTRAINT [PK_protect_hwid] PRIMARY KEY CLUSTERED ([seq] ASC)
    )

    CREATE UNIQUE NONCLUSTERED INDEX [IX_protect_hwid_hash]
        ON [dbo].[tbl_protect_hwid] ([hwid] ASC)

    CREATE NONCLUSTERED INDEX [IX_protect_hwid_account]
        ON [dbo].[tbl_protect_hwid] ([account_id] ASC)

    CREATE NONCLUSTERED INDEX [IX_protect_hwid_blocked]
        ON [dbo].[tbl_protect_hwid] ([is_blocked] ASC)
        WHERE [is_blocked] = 1

    PRINT 'Created table: tbl_protect_hwid'
END
ELSE
    PRINT 'Table tbl_protect_hwid already exists - skipping'
GO

-- ============================================================================
-- 2. Cheat Log Table
-- Stores all cheat detection events for review and appeals
-- ============================================================================

IF NOT EXISTS (SELECT * FROM sysobjects WHERE name = 'tbl_protect_cheat_log' AND xtype = 'U')
BEGIN
    CREATE TABLE [dbo].[tbl_protect_cheat_log] (
        [seq]               BIGINT IDENTITY(1,1) NOT NULL,
        [char_serial]       INT                NOT NULL,
        [char_name]         NVARCHAR(20)       NULL,
        [account_id]        VARCHAR(32)        NULL,
        [hwid]              VARCHAR(128)       NULL,
        [cheat_type]        VARCHAR(32)        NOT NULL,  -- speed_hack, dupe, sqli, etc.
        [score_added]       INT                NOT NULL DEFAULT 0,
        [total_score]       INT                NOT NULL DEFAULT 0,
        [action_taken]      TINYINT            NOT NULL DEFAULT 0, -- 0=none,1=warn,2=kick,3=ban
        [detail]            NVARCHAR(512)      NULL,      -- Additional info
        [pos_x]             FLOAT              NULL,
        [pos_y]             FLOAT              NULL,
        [pos_z]             FLOAT              NULL,
        [map_index]         INT                NULL,
        [log_time]          DATETIME           NOT NULL DEFAULT GETDATE(),
        CONSTRAINT [PK_protect_cheat_log] PRIMARY KEY CLUSTERED ([seq] ASC)
    )

    CREATE NONCLUSTERED INDEX [IX_cheat_log_serial]
        ON [dbo].[tbl_protect_cheat_log] ([char_serial] ASC)

    CREATE NONCLUSTERED INDEX [IX_cheat_log_type]
        ON [dbo].[tbl_protect_cheat_log] ([cheat_type] ASC, [log_time] ASC)

    CREATE NONCLUSTERED INDEX [IX_cheat_log_time]
        ON [dbo].[tbl_protect_cheat_log] ([log_time] ASC)

    CREATE NONCLUSTERED INDEX [IX_cheat_log_action]
        ON [dbo].[tbl_protect_cheat_log] ([action_taken] ASC)
        WHERE [action_taken] > 0

    PRINT 'Created table: tbl_protect_cheat_log'
END
ELSE
    PRINT 'Table tbl_protect_cheat_log already exists - skipping'
GO

-- ============================================================================
-- 3. Transaction Lock Audit Table
-- Records all anti-dupe transaction locks for monitoring
-- ============================================================================

IF NOT EXISTS (SELECT * FROM sysobjects WHERE name = 'tbl_protect_transaction_log' AND xtype = 'U')
BEGIN
    CREATE TABLE [dbo].[tbl_protect_transaction_log] (
        [seq]               BIGINT IDENTITY(1,1) NOT NULL,
        [char_serial]       INT                NOT NULL,
        [lock_type]         VARCHAR(20)        NOT NULL,  -- TRADE,TRUNK,POST,UMT,COMBINE,STORE
        [lock_action]       TINYINT            NOT NULL,  -- 1=lock, 2=unlock, 3=denied(already locked)
        [partner_serial]    INT                NULL,      -- For trade: other player
        [log_time]          DATETIME           NOT NULL DEFAULT GETDATE(),
        CONSTRAINT [PK_protect_txn_log] PRIMARY KEY CLUSTERED ([seq] ASC)
    )

    CREATE NONCLUSTERED INDEX [IX_txn_log_serial]
        ON [dbo].[tbl_protect_transaction_log] ([char_serial] ASC, [log_time] ASC)

    CREATE NONCLUSTERED INDEX [IX_txn_log_denied]
        ON [dbo].[tbl_protect_transaction_log] ([lock_action] ASC)
        WHERE [lock_action] = 3

    PRINT 'Created table: tbl_protect_transaction_log'
END
ELSE
    PRINT 'Table tbl_protect_transaction_log already exists - skipping'
GO

-- ============================================================================
-- 4. Protection System Config Overrides (optional, for DB-driven config)
-- Allows GMs to override INI settings via database without server restart
-- ============================================================================

IF NOT EXISTS (SELECT * FROM sysobjects WHERE name = 'tbl_protect_config' AND xtype = 'U')
BEGIN
    CREATE TABLE [dbo].[tbl_protect_config] (
        [config_key]        VARCHAR(64)        NOT NULL,
        [config_value]      VARCHAR(256)       NOT NULL,
        [description]       NVARCHAR(256)      NULL,
        [updated_by]        VARCHAR(32)        NULL,
        [updated_date]      DATETIME           NOT NULL DEFAULT GETDATE(),
        CONSTRAINT [PK_protect_config] PRIMARY KEY CLUSTERED ([config_key] ASC)
    )

    PRINT 'Created table: tbl_protect_config'
END
ELSE
    PRINT 'Table tbl_protect_config already exists - skipping'
GO
