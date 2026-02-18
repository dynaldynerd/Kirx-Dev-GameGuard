#pragma once

#include "IdaCompat.h"
#include "CUnmannedTraderSubClassInfo.h"

class  CUnmannedTraderSubClassInfoDefault : public CUnmannedTraderSubClassInfo
{
public:
  CUnmannedTraderSubClassInfoDefault(unsigned int dwID);
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
};
