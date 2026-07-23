-- ============================================================================
-- BILLING - Cash Shop / Premium Billing Procedures
-- Reference: 6.23_Guard 2/Server/[SqlAddon]/BILLING/
-- Target: BILLING database
-- ============================================================================
-- TYPE 0 = uses tbl_User with UserID/Cash columns
-- TYPE 1 = uses tbl_personal_billing with binary ID
-- These are ADDON procedures only. Run the COMPLETE version from Guard23
-- reference if your BILLING database does not have the base tables yet.
-- ============================================================================
-- Source files in Guard23:
--   BILLING_TYPE_0_ONLY_ADDON.sql  - Add/Reduce balance + Premium for Type 0
--   BILLING_TYPE_0_COMPLETE.sql    - Full billing DB setup for Type 0
--   BILLING_TYPE_1_ONLY_ADDON.sql  - Add/Reduce balance + Premium for Type 1
--   BILLING_TYPE_1_COMPLETE.sql    - Full billing DB setup for Type 1
-- ============================================================================

USE [BILLING]
GO

-- ============================================================================
-- RF_AddBalance - Add cash points to account
-- ============================================================================
IF OBJECT_ID('dbo.RF_AddBalance', 'P') IS NOT NULL
    DROP PROCEDURE [dbo].[RF_AddBalance]
GO

CREATE PROCEDURE [dbo].[RF_AddBalance]
    @id   VARCHAR(17),
    @cash INT
AS
BEGIN
    SET NOCOUNT ON

    IF (@id IS NULL OR @id = '')
        RETURN

    DECLARE @serial INT
    SELECT @serial = ID FROM [dbo].[tbl_User] WHERE UserID = @id

    IF @serial IS NULL
    BEGIN
        INSERT INTO [dbo].[tbl_User](UserID, Cash) VALUES (@id, @cash)
        RETURN
    END

    UPDATE [dbo].[tbl_User] SET Cash = (Cash + @cash) WHERE ID = @serial
END
GO

PRINT 'Created procedure: RF_AddBalance'
GO

-- ============================================================================
-- RF_ReduceBalance - Deduct cash points from account
-- ============================================================================
IF OBJECT_ID('dbo.RF_ReduceBalance', 'P') IS NOT NULL
    DROP PROCEDURE [dbo].[RF_ReduceBalance]
GO

CREATE PROCEDURE [dbo].[RF_ReduceBalance]
    @id   VARCHAR(17),
    @cash INT
AS
BEGIN
    SET NOCOUNT ON
    UPDATE [dbo].[tbl_User] SET Cash = (Cash - @cash)
    WHERE UserID = @id AND Cash >= @cash
END
GO

PRINT 'Created procedure: RF_ReduceBalance'
GO

-- ============================================================================
-- RF_AddPremium - Add premium subscription days
-- ============================================================================
IF OBJECT_ID('dbo.RF_AddPremium', 'P') IS NOT NULL
    DROP PROCEDURE [dbo].[RF_AddPremium]
GO

CREATE PROCEDURE [dbo].[RF_AddPremium]
    @id  VARCHAR(17),
    @day INT
AS
BEGIN
    SET NOCOUNT ON

    DECLARE @s_id     BINARY(13)
    DECLARE @status   INT
    DECLARE @curdate  DATETIME
    DECLARE @enddate  DATETIME

    IF (@id IS NULL OR @id = '')
        RETURN

    SET @curdate = CONVERT(DATETIME, GETDATE())
    SELECT @s_id = ID, @status = BillingType, @enddate = EndDate
    FROM [dbo].[tbl_personal_billing]
    WHERE id = CONVERT(BINARY, @id)

    IF @s_id IS NULL
    BEGIN
        SET @enddate = DATEADD(DAY, @day, @curdate)
        INSERT INTO [dbo].[tbl_personal_billing](ID, BillingType, EndDate)
        VALUES (CONVERT(BINARY, @id), 2, @enddate)
        RETURN
    END

    IF (@status <> 2)
    BEGIN
        -- Not currently premium, start from now
        SET @enddate = DATEADD(DAY, @day, @curdate)
        UPDATE [dbo].[tbl_personal_billing]
        SET BillingType = 2, EndDate = @enddate WHERE ID = @s_id
    END
    ELSE
    BEGIN
        -- Already premium, extend from current end date
        SET @enddate = DATEADD(DAY, @day, @enddate)
        UPDATE [dbo].[tbl_personal_billing]
        SET EndDate = @enddate WHERE ID = @s_id
    END
END
GO

PRINT 'Created procedure: RF_AddPremium'
GO

-- ============================================================================
-- NOTE: For full BILLING database setup (creating tbl_User, tbl_personal_billing
-- from scratch), use the COMPLETE scripts from Guard23 reference:
--   6.23_Guard 2/Server/[SqlAddon]/BILLING/BILLING_TYPE_0_COMPLETE.sql
--   6.23_Guard 2/Server/[SqlAddon]/BILLING/BILLING_TYPE_1_COMPLETE.sql
-- ============================================================================
