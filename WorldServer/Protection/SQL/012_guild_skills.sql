-- ============================================================================
-- GUILD_SKILLS - Guild Skill System
-- Reference: 6.23_Guard 2/Server/[SqlAddon]/GUILD_SKILLS/GUILD_SKILLS.sql
-- Target: RF_World database
-- ============================================================================
-- Tracks guild skill levels purchased with guild points
-- serial = Guild serial (PK)
-- point_exp = Accumulated guild skill experience/points
-- *_lv = Skill level for each guild passive
-- ============================================================================

USE [RF_World]
GO

IF NOT EXISTS (SELECT * FROM sys.objects
               WHERE object_id = OBJECT_ID(N'[dbo].[tbl_guild_skills]') AND type IN (N'U'))
BEGIN
    CREATE TABLE [dbo].[tbl_guild_skills](
        [serial]        INT     NOT NULL PRIMARY KEY,
        [point_exp]     INT     NOT NULL DEFAULT 0,
        [attack_lv]     TINYINT NOT NULL DEFAULT 0,
        [defense_lv]    TINYINT NOT NULL DEFAULT 0,
        [hp_fp_lv]      TINYINT NOT NULL DEFAULT 0,
        [drop_rate_lv]  TINYINT NOT NULL DEFAULT 0,
        [speed_lv]      TINYINT NOT NULL DEFAULT 0
    )
    CREATE INDEX tbl_guild_skills_idx0 ON [dbo].[tbl_guild_skills] (serial)
    PRINT 'Created table: tbl_guild_skills'
END
ELSE
    PRINT 'Table tbl_guild_skills already exists'
GO

-- ============================================================================
-- GUILD_SKILLS_RESET - Reset all guild skills
-- Reference: 6.23_Guard 2/Server/[SqlAddon]/GUILD_SKILLS/GUILD_SKILLS_RESET.sql
-- Run manually when needed:
-- UPDATE [dbo].[tbl_guild_skills]
--   SET [point_exp]=0, [attack_lv]=0, [defense_lv]=0,
--       [hp_fp_lv]=0, [drop_rate_lv]=0, [speed_lv]=0
-- ============================================================================
