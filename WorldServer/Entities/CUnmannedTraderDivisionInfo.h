#pragma once

#include "IdaCompat.h"
#include <vector>

class CLogFile;
class TiXmlElement;
class CUnmannedTraderClassInfo;
class CUnmannedTraderSortType;

class __cppobj CUnmannedTraderDivisionInfo
{
public:
  CUnmannedTraderDivisionInfo(unsigned int dwID, char *szName);
  ~CUnmannedTraderDivisionInfo();

  void CleanUp();
  bool LoadXML(TiXmlElement *pkElement, CLogFile *kLogger);
  unsigned int GetID();
  unsigned long long GetSize();
  bool IsExistSortTypeID(unsigned int dwID);
  bool IsValidID(unsigned int dwID);

  unsigned int m_dwID;
  char m_szName[128];
  std::vector<CUnmannedTraderClassInfo *> m_vecClass;
  std::vector<CUnmannedTraderSortType *> m_vecSortType;
};
