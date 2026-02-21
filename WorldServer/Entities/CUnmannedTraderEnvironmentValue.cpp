#include "pch.h"

#include "CUnmannedTraderEnvironmentValue.h"

const char *CUnmannedTraderEnvironmentValue::Unmanned_Trader_Setting_Xml_File = "./Initialize/UnmendTrader.xml";
unsigned int CUnmannedTraderEnvironmentValue::Unmanned_Trader_Dev_Account_Start_Serial = 2000000000;
unsigned int CUnmannedTraderEnvironmentValue::Unmanned_Trader_Max_Schdule_Cnt = 10;
unsigned int CUnmannedTraderEnvironmentValue::Unmanned_Trader_Check_Schedule_Delay = 1000;
unsigned int CUnmannedTraderEnvironmentValue::Unmanned_Trader_Change_Taxrate_Check_Changed_Day_Delay = 5000;
unsigned int CUnmannedTraderEnvironmentValue::UNMANNEDTRADETRADER_LAZYCLEANER_LOOP_DELAY = 300000;
unsigned __int16 CUnmannedTraderEnvironmentValue::UNMANNEDTRADETRADER_LAZYCLEANER_UPDATE_MAX_RETRY_CNT = 200;
unsigned int CUnmannedTraderEnvironmentValue::Unmanned_Trader_Max_Row_Count_Search = 10;
unsigned int CUnmannedTraderEnvironmentValue::Unmanned_Trader_Nomal_Max_Regist_Item_Cnt = 5;
unsigned __int8 CUnmannedTraderEnvironmentValue::Unmanned_Trader_Default_Sell_Turm = 72;
const char *CUnmannedTraderEnvironmentValue::Unmanned_Trader_Default_Sort_Type_Name = "serial asc";
const char *CUnmannedTraderEnvironmentValue::Unmanned_Trader_Default_Sort_Query = "si.serial asc";
unsigned int CUnmannedTraderEnvironmentValue::Unmanned_Trader_Min_Limit_Empty_Record_Cnt = 1000;
unsigned int CUnmannedTraderEnvironmentValue::Unmanned_Trader_Default_Empty_Record_Cnt = 10000;
unsigned int CUnmannedTraderEnvironmentValue::UNMANNEDTRADETRADEINFO_LOOP_DELAY = 60000;
const char *CUnmannedTraderEnvironmentValue::UNMANNEDTRADETRADEINFO_SAVE_INI_SECTION_NAME = "Unmannedtradetradeinfo";
const char *CUnmannedTraderEnvironmentValue::UNMANNEDTRADETRADEINFO_SAVE_INI_FILE_NAME = "..\\SystemSave\\ServerState.ini";
