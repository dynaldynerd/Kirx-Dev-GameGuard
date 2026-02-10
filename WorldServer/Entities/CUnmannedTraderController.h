#pragma once

#include "IdaCompat.h"
#include "CLogFile.h"
#include "CUnmannedTraderLazyCleaner.h"
#include "CUnmannedTraderTradeInfo.h"

class CRFWorldDatabase;
class CUnmannedTraderRegistItemInfo;
struct _unmannedtrader_reserved_schedule_info;
struct _TRADE_DB_BASE;

class __cppobj CUnmannedTraderController
{
public:
  static CUnmannedTraderController *Instance();
  bool Init();
  bool Load(unsigned __int16 wInx, unsigned int dwSerial, _TRADE_DB_BASE *kInfo);
  unsigned __int8 GetMaxRegistCnt(unsigned __int16 wInx, unsigned int dwSerial);
  const CUnmannedTraderRegistItemInfo *GetRegItemInfo(unsigned __int16 wInx, unsigned int dwSerial);
  void LogOut(unsigned __int16 wInx, unsigned int dwSerial);
  unsigned __int8 SelectSearchList(char *pData, CRFWorldDatabase *pkWorldDB, unsigned __int8 *byProcRet);
  void CompleteSelectSearchList(unsigned __int8 byDBRet, unsigned __int8 byProcRet, char *pLoadData);
  unsigned __int8 UpdateItemState(char *pData);
  unsigned __int8 UpdateRegistItem(char *pData);
  unsigned __int8 UpdateCancelRegist(char *pData);
  unsigned __int8 UpdateTimeOutCancelRegist(char *pData);
  unsigned __int8 UpdateRePrice(char *pData);
  unsigned __int8 SelectBuy(char *pData);
  unsigned __int8 UpdateBuy(char *pData);
  unsigned __int8 UpdateBuyRollBack(char *pData);
  unsigned __int8 UpdateLazyClean(char *pData);
  unsigned __int8 UpdateBuyComplete(char *pData);
  unsigned __int8 UpdateLogInComplete(char *pData);
  unsigned __int8 UpdateReRegist(char *pData);
  unsigned __int8 UpdateCheatRegistTime(char *pData);
  unsigned __int8 UpdateReRegistRollBack(char *pData);

  void CompleteSelectReservedSchedule(unsigned __int8 byRet, _unmannedtrader_reserved_schedule_info *pLoadData);
  void CompleteUpdateState(unsigned __int8 byRet, char *pLoadData);
  void CompleteRegistItem(unsigned __int8 byRet, char *pLoadData);
  void CompleteCancelRegist(unsigned __int8 byRet, char *pLoadData);
  void CompleteTimeOutCancelRegist(unsigned __int8 byRet, char *pLoadData);
  void CompleteReprice(unsigned __int8 byRet, char *pLoadData);
  void CompleteSelectBuyInfo(unsigned __int8 byRet, char *pLoadData);
  void CompleteBuy(unsigned __int8 byRet, char *pLoadData);
  void CompleteBuyRollBack(unsigned __int8 byRet, char *pLoadData);
  void ComleteLazyClean(char *pData);
  void CompleteBuyComplete(char *pData);
  void CompleteLogInCompete(char *pData);
  void CompleteReRegist(char *pLoadData);
  void CompleteUpdateCheatRegistTime(char *pLoadData);
  void CompleteReRegistRollBack(char *pData);

private:
  bool InitLogger();
  unsigned __int8 GetEmptyRecordSerial(unsigned int *dwSerial, bool *pbRecordInserted);
  unsigned __int8 CheckDBItemState(
    unsigned __int8 byType,
    unsigned int dwRegistSerial,
    unsigned __int8 *byState,
    unsigned __int8 *byProcRet);
  void Log(const char *fmt, ...);

public:
  CLogFile *m_pkLogger;
  CLogFile *m_pkServiceLogger;
  CUnmannedTraderTradeInfo m_kTradeInfo;
  CUnmannedTraderLazyCleaner m_kLazyCleaner;
};
