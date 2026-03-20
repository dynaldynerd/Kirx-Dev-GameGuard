ALTER PROCEDURE [dbo].[pSelect_GuildMasterLastConn]
    @serial int,
    @limit bigint
AS
BEGIN
    SET NOCOUNT ON;

    SELECT LastConnTime
    FROM [dbo].[tbl_base]
    WHERE Serial = @serial
      AND DCK = 0
      AND LastConnTime <= @limit;
END
GO
