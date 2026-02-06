#pragma once

#include "IdaCompat.h"
#include "CUnmannedTraderClassInfoTableType.h"
#include "CUnmannedTraderItemCodeInfo.h"

#include <vector>

class __cppobj CUnmannedTraderClassInfoTableCodeType : public CUnmannedTraderClassInfoTableType
{
public:
  CUnmannedTraderClassInfoTableCodeType(unsigned int dwID);
  ~CUnmannedTraderClassInfoTableCodeType() override;

  bool LoadXML(TiXmlElement *elemClass, CLogFile *kLogger, unsigned int dwDivisionID) override;
  bool GetGroupID(
    unsigned char byTableCode,
    unsigned short wItemTableIndex,
    unsigned char *byClass,
    unsigned char *bySubClass) override;
  bool GetGroupID(
    unsigned char byTableCode,
    unsigned short wItemTableIndex,
    unsigned char *byClass) override;
  CUnmannedTraderClassInfo *Create(unsigned int dwID) override;

  std::vector<CUnmannedTraderItemCodeInfo> m_vecCodeList;
};
