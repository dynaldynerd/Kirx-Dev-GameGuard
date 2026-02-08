#pragma once

#include "IdaCompat.h"
#include "CLogFile.h"
#include "CUnmannedTraderLazyCleaner.h"
#include "CUnmannedTraderTradeInfo.h"

class CRFWorldDatabase;
class CUnmannedTraderRegistItemInfo;

class __cppobj CUnmannedTraderController
{
public:
  static CUnmannedTraderController *Instance();
  bool Init();
  unsigned __int8 GetMaxRegistCnt(unsigned __int16 wInx, unsigned int dwSerial);
  const CUnmannedTraderRegistItemInfo *GetRegItemInfo(unsigned __int16 wInx, unsigned int dwSerial);
  void LogOut(unsigned __int16 wInx, unsigned int dwSerial);
  unsigned __int8 SelectSearchList(char *pData, CRFWorldDatabase *pkWorldDB, unsigned __int8 *byProcRet);
  void CompleteSelectSearchList(unsigned __int8 byDBRet, unsigned __int8 byProcRet, char *pLoadData);

private:
  bool InitLogger();

public:
  CLogFile *m_pkLogger;
  CLogFile *m_pkServiceLogger;
  CUnmannedTraderTradeInfo m_kTradeInfo;
  CUnmannedTraderLazyCleaner m_kLazyCleaner;
};
