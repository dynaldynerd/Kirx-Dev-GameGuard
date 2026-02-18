#pragma once

#include "IdaCompat.h"
#include "CUnmannedTraderItemCodeInfo.h"
#include "CUnmannedTraderSubClassInfo.h"
#include <vector>

class  CUnmannedTraderSubClassInfoCode : public CUnmannedTraderSubClassInfo
{
public:
  CUnmannedTraderSubClassInfoCode(unsigned int dwID);
  ~CUnmannedTraderSubClassInfoCode() override;

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

  std::vector<CUnmannedTraderItemCodeInfo> m_vecCodeList;
};
