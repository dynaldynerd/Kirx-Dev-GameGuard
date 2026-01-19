#pragma once

#include "IdaCompat.h"

class CLogFile;
struct TiXmlElement;

class __cppobj CUnmannedTraderClassInfo
{
public:
  virtual ~CUnmannedTraderClassInfo() = default;

  virtual bool LoadXML(TiXmlElement *pElement, CLogFile *pLogger, unsigned int dwVersion);
  virtual bool GetGroupID(unsigned char byDivision, unsigned short wGroup, unsigned char *pOutMajor, unsigned char *pOutMinor);
  virtual bool GetGroupID(unsigned char byDivision, unsigned short wGroup, unsigned char *pOutMajor);
  virtual bool IsExistGroupID(unsigned char byDivision, unsigned char byGroup);
  virtual CUnmannedTraderClassInfo *Create(unsigned int dwID);

public:
  unsigned int m_dwID;
  unsigned int m_dwVer;
  char m_szTypeName[128];
  char m_szClassName[128];
};
