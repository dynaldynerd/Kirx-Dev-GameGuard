USE [master]
GO

IF DB_ID(N'Billing') IS NULL
BEGIN
    CREATE DATABASE [Billing];
END
GO

USE [Billing]
GO

IF OBJECT_ID(N'[dbo].[tbl_cashList]', N'U') IS NULL
BEGIN
    CREATE TABLE [dbo].[tbl_cashList](
        [id] [varchar](7) NOT NULL,
        [name] [varchar](50) NOT NULL,
        [cost] [nchar](10) NOT NULL,
        CONSTRAINT [PK_tbl_cashList] PRIMARY KEY CLUSTERED (
            [id] ASC,
            [name] ASC
        )
    );
END
GO

IF OBJECT_ID(N'[dbo].[tbl_CashLog]', N'U') IS NULL
BEGIN
    CREATE TABLE [dbo].[tbl_CashLog](
        [Serial] [int] IDENTITY(1,1) NOT NULL,
        [Account] [varchar](20) NOT NULL,
        [Char] [varchar](30) NOT NULL,
        [ItemID] [varchar](7) NOT NULL,
        [Amount] [int] NOT NULL,
        [Server] [varchar](32) NOT NULL,
        [UID] [bigint] NOT NULL,
        [Cost] [int] NOT NULL,
        [Discount] [float] NOT NULL,
        [Return] [tinyint] NOT NULL,
        CONSTRAINT [PK_tbl_CashLog] PRIMARY KEY CLUSTERED (
            [Serial] ASC
        )
    );
END
GO

IF OBJECT_ID(N'[dbo].[tbl_catalog]', N'U') IS NULL
BEGIN
    CREATE TABLE [dbo].[tbl_catalog](
        [idx] [int] IDENTITY(1,1) NOT NULL,
        [tableCode] [int] NOT NULL,
        [itemCode] [varchar](8) NOT NULL,
        [icon] [int] NOT NULL,
        [name] [varchar](64) NOT NULL,
        [K] [int] NOT NULL,
        [D] [bigint] NOT NULL,
        [U] [int] NOT NULL,
        [T] [int] NOT NULL,
        [Price] [int] NOT NULL
    );
END
GO

IF OBJECT_ID(N'[dbo].[tbl_paylog]', N'U') IS NULL
BEGIN
    CREATE TABLE [dbo].[tbl_paylog](
        [PayID] [bigint] NOT NULL,
        [UserID] [varchar](24) NULL,
        [sum] [float] NOT NULL,
        [accept] [bit] NOT NULL CONSTRAINT [DF_tbl_paylog_accept] DEFAULT ((0)),
        CONSTRAINT [PK_tbl_paylog] PRIMARY KEY CLUSTERED (
            [PayID] ASC
        )
    );
END
GO

IF OBJECT_ID(N'[dbo].[tbl_UserStatus]', N'U') IS NULL
BEGIN
    CREATE TABLE [dbo].[tbl_UserStatus](
        [serial] [int] IDENTITY(1,1) NOT NULL,
        [id] [varchar](16) COLLATE Latin1_General_BIN2 NOT NULL,
        [Status] [int] NOT NULL CONSTRAINT [DF_tbl_UserStatus_Status] DEFAULT ((1)),
        [DTStartPrem] [datetime] NULL CONSTRAINT [DF_tbl_UserStatus_DTStartPrem] DEFAULT (getdate()),
        [DTEndPrem] [datetime] NULL CONSTRAINT [DF_tbl_UserStatus_DTEndPrem] DEFAULT (getdate()),
        [Cash] [int] NOT NULL CONSTRAINT [DF_tbl_UserStatus_Cash] DEFAULT ((0)),
        CONSTRAINT [PK_tbl_UserStatus] PRIMARY KEY CLUSTERED (
            [id] ASC
        )
    );
END
GO

IF OBJECT_ID(N'[dbo].[commit_pay]', N'P') IS NOT NULL
BEGIN
    DROP PROCEDURE [dbo].[commit_pay];
END
GO

SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE PROCEDURE [dbo].[commit_pay]
    @inv_id bigint,
    @accid varchar(24),
    @summ int,
    @rw char(1) OUTPUT
AS
DECLARE @a bit;
DECLARE @p bigint;

SELECT @p = PayID, @a = accept
FROM dbo.tbl_paylog
WHERE PayID = @inv_id;

UPDATE dbo.tbl_paylog
SET UserID = @accid
WHERE PayID = @inv_id;

SET @rw = 'A';

IF @a = CONVERT(bit, 0) AND @p <> 0
BEGIN
    UPDATE dbo.tbl_UserStatus
    SET Cash = Cash + @summ
    WHERE ID = @accid;

    UPDATE dbo.tbl_paylog
    SET accept = CONVERT(bit, 1)
    WHERE PayID = @inv_id;

    SET @rw = 'T';
    RETURN;
END

IF @p IS NULL
BEGIN
    SET @rw = 'F';
    RETURN;
END

IF @a IS NULL AND @p <> NULL
BEGIN
    SET @rw = 'C';
    RETURN;
END
GO

IF OBJECT_ID(N'[dbo].[prc_RFONLINE_AVG_EVENT]', N'P') IS NOT NULL
BEGIN
    DROP PROCEDURE [dbo].[prc_RFONLINE_AVG_EVENT];
END
GO

SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE PROCEDURE [dbo].[prc_RFONLINE_AVG_EVENT]
    @t_amount int OUTPUT
AS
BEGIN
    SET NOCOUNT ON;
    SET @t_amount = 1000;
END
GO

IF OBJECT_ID(N'[dbo].[RF_ActivatePremium]', N'P') IS NOT NULL
BEGIN
    DROP PROCEDURE [dbo].[RF_ActivatePremium];
END
GO

SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE PROCEDURE [dbo].[RF_ActivatePremium]
    @UserID varchar(16),
    @months tinyint,
    @ret char(1) OUTPUT
AS
BEGIN
    SET NOCOUNT ON;

    DECLARE @serial int;
    DECLARE @plusdate datetime;

    SET @serial = 0;

    SELECT @serial = serial
    FROM tbl_UserStatus
    WHERE id = @UserID;

    IF @serial < 1
    BEGIN
        SET @ret = 'F';
        RETURN;
    END

    SET @plusdate = DATEADD(mm, @months, GETDATE());

    UPDATE tbl_UserStatus
    SET [Status] = 2,
        DTStartPrem = GETDATE(),
        DTEndPrem = @plusdate
    WHERE serial = @serial;

    IF @@ERROR = 0
    BEGIN
        SET @ret = 'T';
        RETURN;
    END
END
GO

IF OBJECT_ID(N'[dbo].[RF_ChargeBalance]', N'P') IS NOT NULL
BEGIN
    DROP PROCEDURE [dbo].[RF_ChargeBalance];
END
GO

SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE PROCEDURE [dbo].[RF_ChargeBalance]
    @Login varchar(32),
    @Char varchar(20),
    @ItemID varchar(20),
    @Amount int,
    @Server varchar(20),
    @UID bigint,
    @Cost varchar(20),
    @Discount int,
    @ret int OUTPUT
AS
SET NOCOUNT ON;

DECLARE @totalAmount int;
DECLARE @ProductAmount int;
DECLARE @cashBalance int;

INSERT INTO [dbo].[tbl_CashLog] ([Account], [Char], [ItemID], [Amount], [Server], [UID], [Cost], [Discount], [Return])
VALUES (@Login, @Char, @ItemID, @Amount, @Server, @UID, @Cost, @Discount, 0);

IF @@ERROR <> 0
BEGIN
    SET @ret = 0;
    RETURN;
END
ELSE IF @Login IS NULL OR RTRIM(@Login) = ''
BEGIN
    SET @ret = 0;
    RETURN;
END
ELSE IF @Cost IS NULL OR @Cost <= '0'
BEGIN
    SET @ret = 0;
    RETURN;
END
ELSE IF @Amount IS NULL OR @Amount <= '0'
BEGIN
    SET @ret = 0;
    RETURN;
END
ELSE IF @Discount IS NULL OR @Discount < '0' OR @Discount >= '100'
BEGIN
    SET @ret = 0;
    RETURN;
END
ELSE IF @ItemID IS NULL OR RTRIM(@ItemID) = ''
BEGIN
    SET @ret = 0;
    RETURN;
END
ELSE IF @Server IS NULL OR RTRIM(@Server) = ''
BEGIN
    SET @ret = 0;
    RETURN;
END
ELSE IF @Char IS NULL OR RTRIM(@Char) = ''
BEGIN
    SET @ret = 0;
    RETURN;
END

SELECT @cashBalance = Cash
FROM tbl_UserStatus
WHERE ID = @Login;

/*
SELECT @productAmount = cost
FROM tbl_cashList
WHERE id = @ItemID;

IF @productAmount != @cost OR @Cost <= 0 OR @productAmount <= 0 OR @productAmount IS NULL
BEGIN
    SET @ret = 0;
    RETURN;
END
*/

IF @Discount = 0
BEGIN
    SET @totalAmount = @Cost * @Amount;
END
ELSE
BEGIN
    SET @totalAmount = CONVERT(int, (@Cost * @Amount) / @Discount);
END

IF @totalAmount > @CashBalance
BEGIN
    SET @ret = 0;
    RETURN;
END

SET @cashBalance = @cashBalance - @totalAmount;

UPDATE tbl_UserStatus
SET Cash = @cashBalance
WHERE ID = @Login;

IF @@ERROR <> 0
BEGIN
    SET @ret = 0;
    RETURN;
END
ELSE
BEGIN
    SET @ret = 1;
END

UPDATE [dbo].[tbl_CashLog]
SET [Return] = @ret
WHERE Account = @Login;
GO

IF OBJECT_ID(N'[dbo].[RF_CheckAccountStatus]', N'P') IS NOT NULL
BEGIN
    DROP PROCEDURE [dbo].[RF_CheckAccountStatus];
END
GO

SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE PROCEDURE [dbo].[RF_CheckAccountStatus]
    @id varchar(16),
    @Status int OUTPUT
AS
BEGIN
    SET NOCOUNT ON;

    DECLARE @EndDt datetime;

    SELECT @EndDt = DTEndPrem
    FROM tbl_UserStatus
    WHERE id = @id;

    IF @EndDt > GETDATE()
    BEGIN
        SELECT @status = [Status]
        FROM tbl_UserStatus
        WHERE id = @id;
    END
    ELSE
    BEGIN
        SET @Status = 1;

        UPDATE tbl_UserStatus
        SET [Status] = @status
        WHERE id = @id;
    END
END
GO

IF OBJECT_ID(N'[dbo].[RF_CheckBalance]', N'P') IS NOT NULL
BEGIN
    DROP PROCEDURE [dbo].[RF_CheckBalance];
END
GO

SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE PROCEDURE [dbo].[RF_CheckBalance]
    @s_userid varchar(50),
    @s_amount int OUTPUT
AS
BEGIN
    SET NOCOUNT ON;

    SELECT @s_amount = Cash
    FROM tbl_UserStatus WITH (READUNCOMMITTED)
    WHERE ID = @s_userid;

    IF @s_amount IS NULL
    BEGIN
        SET @s_amount = 0;
    END
END
GO

IF OBJECT_ID(N'[dbo].[RF_PING]', N'P') IS NOT NULL
BEGIN
    DROP PROCEDURE [dbo].[RF_PING];
END
GO

SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE PROCEDURE [dbo].[RF_PING]
AS
BEGIN
    SET NOCOUNT ON;
END
GO

IF OBJECT_ID(N'[dbo].[SP_RF_CHG_ITEM_GAMEON]', N'P') IS NOT NULL
BEGIN
    DROP PROCEDURE [dbo].[SP_RF_CHG_ITEM_GAMEON];
END
GO

SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE PROCEDURE [dbo].[SP_RF_CHG_ITEM_GAMEON]
    @uid varchar(32),
    @charname varchar(20),
    @item_price int,
    @ccr_itemcode varchar(7),
    @world varchar(20),
    @ip_addr varchar(16),
    @s_status int OUTPUT,
    @s_amount int OUTPUT
AS
BEGIN
    SET NOCOUNT ON;

    SET @s_amount = 10000;
    SET @s_status = 0;
END
GO
