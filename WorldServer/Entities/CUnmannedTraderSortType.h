#pragma once

#include "IdaCompat.h"

class CLogFile;
class TiXmlElement;

class __cppobj CUnmannedTraderSortType
{
public:
  CUnmannedTraderSortType(unsigned int dwID);
  bool LoadXML(TiXmlElement *pkElemSortType, CLogFile *kLogger, unsigned int dwDivisionID);
  unsigned int GetID();
  char *GetQuery();
  const CUnmannedTraderSortType *operator=(const CUnmannedTraderSortType *lhs);

  unsigned int m_dwID;
  char m_szTypeName[128];
  char m_szQuery[128];
};
