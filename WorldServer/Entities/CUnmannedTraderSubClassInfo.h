#pragma once

#include "IdaCompat.h"

class CLogFile;
class TiXmlElement;

class __cppobj CUnmannedTraderSubClassInfo
{
public:
  virtual ~CUnmannedTraderSubClassInfo();

  virtual CUnmannedTraderSubClassInfo *Create(unsigned int dwID) = 0;
  virtual bool LoadXML(
    TiXmlElement *pkElement,
    CLogFile *kLogger,
    unsigned int dwDivisionID,
    unsigned int dwClassID);
  virtual bool GetGroupID(
    unsigned char byTableCode,
    unsigned short wItemTableIndex,
    unsigned char *bySubClass);

  CUnmannedTraderSubClassInfo(unsigned int dwID);
  CUnmannedTraderSubClassInfo(const CUnmannedTraderSubClassInfo *lhs);
  const CUnmannedTraderSubClassInfo *Copy(const CUnmannedTraderSubClassInfo *lhs);
  unsigned int GetID();
  char *GetTypeName();

  char m_szName[24];
  unsigned int m_dwID;
};
