SET XACT_ABORT ON;
BEGIN TRY
    BEGIN TRAN;

    DECLARE @Targets TABLE (
        SchemaName sysname NOT NULL,
        TableName  sysname NOT NULL,
        ColumnName sysname NOT NULL
    );

    -- Directly traced columns using YYMMDDHHMM compact timestamp format.
    INSERT INTO @Targets (SchemaName, TableName, ColumnName)
    VALUES
        (N'dbo', N'tbl_base',            N'LastConnTime'),
        (N'dbo', N'tbl_TimeLimitInfo',   N'LastLogoutTime'),
        (N'dbo', N'tbl_PrimiumPlayTime', N'LastConnTime'),
        (N'dbo', N'tbl_supplement',      N'BufEndTime'),
        (N'dbo', N'tbl_unit',            N'Cut_0'),
        (N'dbo', N'tbl_unit',            N'Cut_1'),
        (N'dbo', N'tbl_unit',            N'Cut_2'),
        (N'dbo', N'tbl_unit',            N'Cut_3');

    -- tbl_npc_quest_history Time1..Time70
    ;WITH N AS (
        SELECT TOP (70) ROW_NUMBER() OVER (ORDER BY (SELECT NULL)) AS n
        FROM sys.all_objects
    )
    INSERT INTO @Targets (SchemaName, TableName, ColumnName)
    SELECT N'dbo', N'tbl_npc_quest_history', N'Time' + CONVERT(varchar(3), n)
    FROM N;

    DECLARE @sql nvarchar(max) = N'';
    DECLARE @dropDefaultSql nvarchar(max) = N'';
    DECLARE @addDefaultSql nvarchar(max) = N'';

    DECLARE @Defaults TABLE (
        SchemaName sysname NOT NULL,
        TableName sysname NOT NULL,
        ColumnName sysname NOT NULL,
        ConstraintName sysname NOT NULL,
        Definition nvarchar(max) NOT NULL
    );

    -- Capture default constraints on target columns so they can be restored.
    INSERT INTO @Defaults (SchemaName, TableName, ColumnName, ConstraintName, Definition)
    SELECT
        s.name,
        t.name,
        c.name,
        dc.name,
        dc.definition
    FROM @Targets trg
    JOIN sys.schemas s
        ON s.name = trg.SchemaName
    JOIN sys.tables t
        ON t.schema_id = s.schema_id
       AND t.name = trg.TableName
    JOIN sys.columns c
        ON c.object_id = t.object_id
       AND c.name = trg.ColumnName
    JOIN sys.default_constraints dc
        ON dc.parent_object_id = c.object_id
       AND dc.parent_column_id = c.column_id;

    -- Drop defaults before ALTER COLUMN.
    SELECT
        @dropDefaultSql = @dropDefaultSql
                        + N'ALTER TABLE '
                        + QUOTENAME(SchemaName) + N'.' + QUOTENAME(TableName)
                        + N' DROP CONSTRAINT ' + QUOTENAME(ConstraintName)
                        + N';' + CHAR(13) + CHAR(10)
    FROM @Defaults;

    IF LEN(@dropDefaultSql) > 0
    BEGIN
        PRINT N'Dropping default constraints on target columns...';
        EXEC sp_executesql @dropDefaultSql;
    END

    SELECT
        @sql = @sql
             + N'ALTER TABLE '
             + QUOTENAME(s.name) + N'.' + QUOTENAME(t.name)
             + N' ALTER COLUMN ' + QUOTENAME(c.name) + N' BIGINT '
             + CASE WHEN c.is_nullable = 1 THEN N'NULL' ELSE N'NOT NULL' END
             + N';' + CHAR(13) + CHAR(10)
    FROM @Targets trg
    JOIN sys.schemas s
        ON s.name = trg.SchemaName
    JOIN sys.tables t
        ON t.schema_id = s.schema_id
       AND t.name = trg.TableName
    JOIN sys.columns c
        ON c.object_id = t.object_id
       AND c.name = trg.ColumnName
    JOIN sys.types ty
        ON ty.user_type_id = c.user_type_id
    WHERE ty.name <> N'bigint';

    IF LEN(@sql) > 0
    BEGIN
        PRINT N'Applying schema changes...';
        EXEC sp_executesql @sql;
    END
    ELSE
    BEGIN
        PRINT N'No schema changes needed (already BIGINT or columns missing).';
    END

    -- Re-add dropped defaults after ALTER COLUMN.
    SELECT
        @addDefaultSql = @addDefaultSql
                       + N'ALTER TABLE '
                       + QUOTENAME(SchemaName) + N'.' + QUOTENAME(TableName)
                       + N' ADD CONSTRAINT ' + QUOTENAME(ConstraintName)
                       + N' DEFAULT ' + Definition
                       + N' FOR ' + QUOTENAME(ColumnName)
                       + N';' + CHAR(13) + CHAR(10)
    FROM @Defaults;

    IF LEN(@addDefaultSql) > 0
    BEGIN
        PRINT N'Restoring default constraints on target columns...';
        EXEC sp_executesql @addDefaultSql;
    END

    COMMIT TRAN;
END TRY
BEGIN CATCH
    IF @@TRANCOUNT > 0 ROLLBACK TRAN;
    THROW;
END CATCH;
GO

-- Verify changed column types.
SELECT
    s.name  AS SchemaName,
    t.name  AS TableName,
    c.name  AS ColumnName,
    ty.name AS SqlType,
    c.max_length,
    c.is_nullable
FROM sys.schemas s
JOIN sys.tables t ON t.schema_id = s.schema_id
JOIN sys.columns c ON c.object_id = t.object_id
JOIN sys.types ty ON ty.user_type_id = c.user_type_id
WHERE
    (s.name = N'dbo' AND t.name = N'tbl_base'            AND c.name = N'LastConnTime')
 OR (s.name = N'dbo' AND t.name = N'tbl_TimeLimitInfo'   AND c.name = N'LastLogoutTime')
 OR (s.name = N'dbo' AND t.name = N'tbl_PrimiumPlayTime' AND c.name = N'LastConnTime')
 OR (s.name = N'dbo' AND t.name = N'tbl_supplement'      AND c.name = N'BufEndTime')
 OR (s.name = N'dbo' AND t.name = N'tbl_unit'            AND c.name IN (N'Cut_0', N'Cut_1', N'Cut_2', N'Cut_3'))
 OR (s.name = N'dbo' AND t.name = N'tbl_npc_quest_history' AND c.name LIKE N'Time%')
ORDER BY SchemaName, TableName, ColumnName;

-- Optional: inspect race battle proc-backed timestamp usage.
SELECT OBJECT_DEFINITION(OBJECT_ID(N'dbo.pInsert_RaceBattleLog_070427')) AS ProcText;
