SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO

-- Refresh select procedures that project bigint-backed compact YYMMDDHHMM
-- timestamp columns after migrate_korlocaltime_bigint.sql.

ALTER PROCEDURE [dbo].[pSelect_CharacterBaseInfo_20061115]
@serial int
AS
SET NOCOUNT ON

SELECT Name, Slot, Race, Class, Lv,
    Dalant, Gold, BaseShape, LastConnTime,
    EK0, EK1, EK2, EK3, EK4, EK5, EK6, EK7,
    EU0, EU1, EU2, EU3, EU4, EU5, EU6, EU7,
    ES0, ES1, ES2, ES3, ES4, ES5, ES6, ES7,
    ET0, ET1, ET2, ET3, ET4, ET5, ET6, ET7,
    FirstConnTime
from [dbo].[tbl_base] where Serial=@serial and DCK = 0
GO

ALTER PROCEDURE [dbo].[pSelect_CharacterBaseInfoByName_20061115]
@charactername varchar(16)
AS
SET NOCOUNT ON

SELECT Serial, Race, Class, Slot, Lv, Dalant,
    Gold, BaseShape, LastConnTime, Account,
    EK0, EK1, EK2, EK3, EK4, EK5, EK6, EK7,
    EU0, EU1, EU2, EU3, EU4, EU5, EU6, EU7,
    ES0, ES1, ES2, ES3, ES4, ES5, ES6, ES7,
    ET0, ET1, ET2, ET3, ET4, ET5, ET6, ET7
from [dbo].[tbl_base] where Name=@charactername and DCK = 0
GO

ALTER PROCEDURE [dbo].[pSelect_CharacterBaseInfoBySerial_20061115]
@accountserial int
AS
SET NOCOUNT ON

SELECT Serial, Name, Slot, Race, Class, Lv,
    Dalant, Gold, BaseShape, LastConnTime,
    EK0, EK1, EK2, EK3, EK4, EK5, EK6, EK7,
    EU0, EU1, EU2, EU3, EU4, EU5, EU6, EU7,
    ES0, ES1, ES2, ES3, ES4, ES5, ES6, ES7,
    ET0, ET1, ET2, ET3, ET4, ET5, ET6, ET7
from [dbo].[tbl_base] where AccountSerial=@accountserial and DCK = 0
GO

ALTER PROCEDURE [dbo].[pSelect_GuildMasterLastConn]
@serial int,
@limit bigint
AS
SET NOCOUNT ON

SELECT LastConnTime
FROM [dbo].[tbl_base]
WHERE Serial = @serial
AND DCK = 0
AND LastConnTime <= @limit
GO

ALTER PROCEDURE [dbo].[pSelect_NpcQuest_History_20080811]
@serial int
AS
SET NOCOUNT ON

SELECT Code1, Level1, Time1, Code2, Level2, Time2, Code3, Level3, Time3,
 Code4, Level4, Time4, Code5, Level5, Time5, Code6, Level6, Time6,
 Code7, Level7, Time7, Code8, Level8, Time8, Code9, Level9, Time9,
 Code10, Level10, Time10, Code11, Level11, Time11, Code12, Level12, Time12,
 Code13, Level13, Time13, Code14, Level14, Time14, Code15, Level15, Time15,
 Code16, Level16, Time16, Code17, Level17, Time17, Code18, Level18, Time18,
 Code19, Level19, Time19, Code20, Level20, Time20, Code21, Level21, Time21,
 Code22, Level22, Time22, Code23, Level23, Time23, Code24, Level24, Time24,
 Code25, Level25, Time25, Code26, Level26, Time26, Code27, Level27, Time27,
 Code28, Level28, Time28, Code29, Level29, Time29, Code30, Level30, Time30,
 Code31, Level31, Time31, Code32, Level32, Time32, Code33, Level33, Time33,
 Code34, Level34, Time34, Code35, Level35, Time35, Code36, Level36, Time36,
 Code37, Level37, Time37, Code38, Level38, Time38, Code39, Level39, Time39,
 Code40, Level40, Time40, Code41, Level41, Time41, Code42, Level42, Time42,
 Code43, Level43, Time43, Code44, Level44, Time44, Code45, Level45, Time45,
 Code46, Level46, Time46, Code47, Level47, Time47, Code48, Level48, Time48,
 Code49, Level49, Time49, Code50, Level50, Time50, Code51, Level51, Time51,
 Code52, Level52, Time52, Code53, Level53, Time53, Code54, Level54, Time54,
 Code55, Level55, Time55, Code56, Level56, Time56, Code57, Level57, Time57,
 Code58, Level58, Time58, Code59, Level59, Time59, Code60, Level60, Time60,
 Code61, Level61, Time61, Code62, Level62, Time62, Code63, Level63, Time63,
 Code64, Level64, Time64, Code65, Level65, Time65, Code66, Level66, Time66,
 Code67, Level67, Time67, Code68, Level68, Time68, Code69, Level69, Time69,
 Code70, Level70, Time70
FROM [dbo].[tbl_npc_quest_history]
WHERE Serial = @serial
GO

ALTER PROCEDURE [dbo].[pSelect_PrimiumPlayTime]
@serial int
AS
SET NOCOUNT ON

select [LastConnTime], [ContPlayTime], [ForcedClose], [ReceiveCoupon], [EnsureTime]
from [dbo].[tbl_PrimiumPlayTime] where AccountSerial = @serial
GO

ALTER PROCEDURE [dbo].[pSelect_Supplement_20080428]
@aserial int
AS
SET NOCOUNT ON

select PvpPointLeak, LastAttBuff, BufEndTime, RaceBuffClear
from tbl_supplement
where Serial=@aserial and DCK=0
GO

ALTER PROCEDURE [dbo].[pSelect_Unit]
@serial int
AS
SET NOCOUNT ON

SELECT F_0, F_1, F_2, F_3,
    H_0, H_1, H_2, H_3,
    U_0, U_1, U_2, U_3,
    L_0, L_1, L_2, L_3,
    A_0, A_1, A_2, A_3,
    S_0, S_1, S_2, S_3,
    B_0, B_1, B_2, B_3,
    Gg_0, Gg_1, Gg_2, Gg_3,
    AB_0, AB_1, AB_2, AB_3,
    SB_0, SB_1, SB_2, SB_3,
    KF_0, KF_1, KF_2, KF_3,
    PF_0, PF_1, PF_2, PF_3,
    Cut_0, Cut_1, Cut_2, Cut_3,
    SP0_0, SP0_1, SP0_2, SP0_3,
    SP1_0, SP1_1, SP1_2, SP1_3,
    SP2_0, SP2_1, SP2_2, SP2_3,
    SP3_0, SP3_1, SP3_2, SP3_3,
    SP4_0, SP4_1, SP4_2, SP4_3,
    SP5_0, SP5_1, SP5_2, SP5_3,
    SP6_0, SP6_1, SP6_2, SP6_3,
    SP7_0, SP7_1, SP7_2, SP7_3
FROM [dbo].[tbl_unit] WHERE Serial=@serial
GO
