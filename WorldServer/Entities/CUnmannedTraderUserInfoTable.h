#pragma once

#include "IdaCompat.h"
#include "CLogFile.h"
#include "CUnmannedTraderUserInfo.h"
#include <vector>

class __cppobj CUnmannedTraderUserInfoTable
{
public:
  static CUnmannedTraderUserInfoTable *Instance();

  CUnmannedTraderUserInfoTable();
  bool Init();
  void SetLogger(CLogFile *pkLogger, CLogFile *pkServiceLogger);
  CUnmannedTraderUserInfo *Find(unsigned int dwSerial);
  CUnmannedTraderUserInfo *FindByIndex(unsigned __int16 wInx);
  CUnmannedTraderUserInfo *FindUser(unsigned __int16 wInx, unsigned int dwSerial);
  unsigned __int8 GetMaxRegistCnt(unsigned __int16 wInx, unsigned int dwSerial);
  const CUnmannedTraderRegistItemInfo *GetRegItemInfo(unsigned __int16 wInx, unsigned int dwSerial);
  void CompleteSearch(unsigned __int8 byDBRet, unsigned __int8 byProcRet, char *pLoadData);
  void LogOut(unsigned __int16 wInx, unsigned int dwSerial);

  static CUnmannedTraderUserInfoTable *ms_Instance;

  CLogFile *m_pkLogger;
  CLogFile *m_pkServiceLogger;
  std::vector<CUnmannedTraderUserInfo> m_veckInfo;
};
