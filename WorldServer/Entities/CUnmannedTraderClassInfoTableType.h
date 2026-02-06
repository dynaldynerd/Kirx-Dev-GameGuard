#pragma once

#include "IdaCompat.h"
#include "CUnmannedTraderClassInfo.h"

#include <vector>

class CUnmannedTraderSubClassInfo;

class __cppobj CUnmannedTraderClassInfoTableType : public CUnmannedTraderClassInfo
{
public:
  CUnmannedTraderClassInfoTableType(unsigned int dwID);
  ~CUnmannedTraderClassInfoTableType() override;

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
  bool IsExistGroupID(unsigned char byClass, unsigned char bySubClass) override;
  CUnmannedTraderClassInfo *Create(unsigned int dwID) override;

  bool IsValidID(unsigned int dwID);
  void CleanUp();

  unsigned char m_byTableCode;
  std::vector<CUnmannedTraderSubClassInfo *> m_vecSubClass;
};
