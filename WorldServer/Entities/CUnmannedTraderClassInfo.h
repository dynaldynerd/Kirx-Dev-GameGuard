#pragma once

#include "IdaCompat.h"

class CLogFile;
class TiXmlElement;

class  CUnmannedTraderClassInfo
{
public:
  virtual ~CUnmannedTraderClassInfo();

  virtual bool LoadXML(TiXmlElement *pElement, CLogFile *pLogger, unsigned int dwVersion) = 0;
  virtual bool GetGroupID(
    unsigned char byDivision,
    unsigned short wGroup,
    unsigned char *pOutMajor,
    unsigned char *pOutMinor) = 0;
  virtual bool GetGroupID(
    unsigned char byDivision,
    unsigned short wGroup,
    unsigned char *pOutMajor) = 0;
  virtual bool IsExistGroupID(unsigned char byClass, unsigned char bySubClass) = 0;
  virtual CUnmannedTraderClassInfo *Create(unsigned int dwID) = 0;

  CUnmannedTraderClassInfo(unsigned int dwID);
  const CUnmannedTraderClassInfo *Copy(const CUnmannedTraderClassInfo *lhs);
  unsigned int GetID();
  char *GetTypeName();

public:
  unsigned int m_dwID;
  unsigned int m_dwVer;
  char m_szTypeName[128];
  char m_szClassName[128];
};
