#pragma once

#include "IdaCompat.h"
#include "CLogFile.h"
#include <vector>

class CUnmannedTraderDivisionInfo;

class __cppobj CUnmannedTraderGroupIDInfo
{
public:
  CUnmannedTraderGroupIDInfo();
  ~CUnmannedTraderGroupIDInfo();

  void CleanUp();
  void SetLogger(CLogFile *pkLogger);
  void Log(const char *fmt, ...);
  bool IsExistID(unsigned int dwID);
  bool GetIDInfo(std::vector<std::pair<unsigned long, unsigned long>> *vecInfo);
  bool LoadXML(const char *szFileName);

  CLogFile *m_pkLogger;
  std::vector<CUnmannedTraderDivisionInfo *> m_vecDivisionInfo;
};
