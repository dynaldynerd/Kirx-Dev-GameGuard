-- ============================================================================
-- SAVE_HWID - Hardware ID Tracking on Account Table
-- Reference: 6.23_Guard 2/Server/[SqlAddon]/SAVE_HWID.sql
-- Target: RF_User database
-- ============================================================================
-- Adds lastconnecthwid column to tbl_UserAccount and tbl_StaffAccount
-- Modifies logout log procedure to include HWID in daily logs
-- ============================================================================

USE [RF_User]
GO

-- Add HWID column to user accounts
IF NOT EXISTS (SELECT 1 FROM INFORMATION_SCHEMA.COLUMNS
               WHERE TABLE_NAME = 'tbl_UserAccount' AND COLUMN_NAME = 'lastconnecthwid')
BEGIN
    ALTER TABLE [dbo].[tbl_UserAccount] ADD lastconnecthwid VARCHAR(128) NULL
    PRINT 'Added lastconnecthwid to tbl_UserAccount'
END
GO

-- Add HWID column to staff/GM accounts
IF NOT EXISTS (SELECT 1 FROM INFORMATION_SCHEMA.COLUMNS
               WHERE TABLE_NAME = 'tbl_StaffAccount' AND COLUMN_NAME = 'lastconnecthwid')
BEGIN
    ALTER TABLE [dbo].[tbl_StaffAccount] ADD lastconnecthwid VARCHAR(128) NULL
    PRINT 'Added lastconnecthwid to tbl_StaffAccount'
END
GO

-- Procedure: Update HWID on login
-- Staff accounts have serial >= 2000000000
IF OBJECT_ID('dbo.pUpdate_HWID_Update', 'P') IS NOT NULL
    DROP PROCEDURE [dbo].[pUpdate_HWID_Update]
GO

CREATE PROCEDURE [dbo].[pUpdate_HWID_Update]
    @serial INT,
    @hwid   VARCHAR(MAX)
AS
BEGIN
    SET NOCOUNT ON
    IF @serial >= 2000000000
        UPDATE [dbo].[tbl_StaffAccount] SET lastconnecthwid = @hwid WHERE serial = @serial
    ELSE
        UPDATE [dbo].[tbl_UserAccount] SET lastconnecthwid = @hwid WHERE serial = @serial
END
GO

PRINT 'Created procedure: pUpdate_HWID_Update'
GO

-- Procedure: Logout log with HWID (replaces original)
-- Dynamically adds HardwareId column to daily logout log table if missing
IF OBJECT_ID('dbo.pInsert_UserLogoutLog_Daily20070115', 'P') IS NOT NULL
    DROP PROCEDURE [dbo].[pInsert_UserLogoutLog_Daily20070115]
GO

CREATE PROCEDURE [dbo].[pInsert_UserLogoutLog_Daily20070115]
    @serial      INT,
    @loginDate   VARCHAR(16),
    @ip          VARCHAR(16),
    @worldcode   INT,
    @billingtype INT,
    @level       INT,
    @charSerial  INT,
    @Account     VARCHAR(17)
AS
BEGIN
    SET NOCOUNT ON

    DECLARE
        @today     VARCHAR(8),
        @tableName NVARCHAR(64),
        @hwid      VARCHAR(128),
        @strSql    NVARCHAR(MAX)

    SET @today = CONVERT(VARCHAR(8), GETDATE(), 112)
    SET @tableName = QUOTENAME('tbl_UserLogout_Log' + @today)

    -- Get stored HWID
    IF @serial >= 2000000000
        SELECT @hwid = lastconnecthwid FROM tbl_StaffAccount WHERE serial = @serial
    ELSE
        SELECT @hwid = lastconnecthwid FROM tbl_UserAccount WHERE serial = @serial

    -- Add HardwareId column to daily table if not present
    IF COL_LENGTH('tbl_UserLogout_Log' + @today, 'HardwareId') IS NULL
    BEGIN
        SET @strSql = N'ALTER TABLE ' + @tableName + ' ADD HardwareId VARCHAR(128) NULL;'
        EXEC sp_executesql @strSql
    END

    -- Insert logout log with HWID
    SET @strSql = N'INSERT INTO tbl_UserLogout_Log' + @today
        + '(nAccountSerial, dtLoginDate, dtLogoutDate, ip, nWorldCode, nBillingType, nLevel, nAvatorSerial, Account, HardwareId) '
        + N' VALUES( ' + CONVERT(VARCHAR, @serial) + ',' + '''' + @loginDate + '''' + ','
        + N'''' + CONVERT(VARCHAR(32), GETDATE(), 20) + '''' + ',' + '''' + @ip + '''' + ',' + CONVERT(VARCHAR, @worldcode) + ','
        + CONVERT(VARCHAR, @billingtype) + ',' + CONVERT(VARCHAR, @level) + ','
        + CONVERT(VARCHAR, @charSerial) + ',' + '''' + @Account + '''' + ',' + '''' + ISNULL(@hwid, '') + '''' + ') '
    EXEC sp_executesql @strSql

    SET NOCOUNT OFF
END
GO

PRINT 'Created procedure: pInsert_UserLogoutLog_Daily20070115 (with HWID)'
GO
