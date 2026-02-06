#pragma once

#include "IdaCompat.h"
#include "CUnmannedTraderSubClassInfo.h"

class __cppobj CUnmannedTraderSubClassInfoLevel : public CUnmannedTraderSubClassInfo
{
public:
  CUnmannedTraderSubClassInfoLevel(unsigned int dwID);
  ~CUnmannedTraderSubClassInfoLevel() override;

  CUnmannedTraderSubClassInfo *Create(unsigned int dwID) override;
  bool LoadXML(
    TiXmlElement *elemSubClass,
    CLogFile *kLogger,
    unsigned int dwDivisionID,
    unsigned int dwClassID) override;
  bool GetGroupID(
    unsigned char byTableCode,
    unsigned short wItemTableIndex,
    unsigned char *bySubClass) override;

  unsigned char m_byMin;
  unsigned char m_byMax;
};
