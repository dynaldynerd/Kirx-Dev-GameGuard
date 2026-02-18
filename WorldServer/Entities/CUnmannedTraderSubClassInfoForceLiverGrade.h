#pragma once

#include "IdaCompat.h"
#include "CUnmannedTraderSubClassInfo.h"

class  CUnmannedTraderSubClassInfoForceLiverGrade : public CUnmannedTraderSubClassInfo
{
public:
  CUnmannedTraderSubClassInfoForceLiverGrade(unsigned int dwID);
  ~CUnmannedTraderSubClassInfoForceLiverGrade() override;

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

  unsigned char m_byGrade;
};
